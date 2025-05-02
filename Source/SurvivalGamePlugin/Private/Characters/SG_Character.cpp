// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SG_Character.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "Net/UnrealNetwork.h"
#include "InputActionValue.h"

#include "Components/SG_EnhancedInputComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "Animation/SG_AnimInstance.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "AI/SG_AIController.h"
#include "UI/HUD/InGameHUD.h"

DEFINE_LOG_CATEGORY(LogSGCharacter);

ASG_Character::ASG_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	bFindCameraComponentWhenViewTarget = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	GetCharacterMovement()->JumpZVelocity = 200.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 0.f; 
	CameraBoom->bUsePawnControlRotation = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(CameraBoom);
	FirstPersonCamera->bUsePawnControlRotation = false;

	AbilitySystemComponent = CreateDefaultSubobject<USG_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<USG_CharacterAttributeSet>(TEXT("AttributeSet"));
}

void ASG_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ASG_Character, FocusedInteractableObject, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ASG_Character, bIsMirrored, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ASG_Character, ControlRotation, COND_None, REPNOTIFY_Always);
}

void ASG_Character::UpdateCamera(float Alpha)
{
	APlayerController* PlayerCont = Cast<APlayerController>(GetController());
	if (IsValid(PlayerCont) == false)
	{
		return;
	}

	FQuat NewControlRotation = FQuat::Slerp(PlayerCont->GetControlRotation().Quaternion(), GetActorQuat(), Alpha);
	PlayerCont->SetControlRotation(FRotator(NewControlRotation));
	SetRepControlRotation(GetControlRotation());
}

void ASG_Character::Server_SetFocusedInteractable_Implementation(UObject* InFocusedInteractable)
{
	SetFocusedInteractable(InFocusedInteractable);
}

void ASG_Character::SetFocusedInteractable(UObject* InFocusedInteractable)
{
	if (HasAuthority() == false)
	{
		Server_SetFocusedInteractable(InFocusedInteractable);
		return;
	}
	FocusedInteractableObject = InFocusedInteractable;
}

void ASG_Character::EmitInteractionChecker()
{
	FVector StartTrace = GetFirstPersonCamera()->GetComponentLocation();
	FVector EndTrace = GetFirstPersonCamera()->GetForwardVector() * CurrentInteractionRange + StartTrace;

	const FName TraceTag("InteractableTraceTag");

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::EmitInteractionChecker IsValid(World) == false"));
		return;
	}

	FHitResult HitResult;
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);
	CQP.TraceTag = TraceTag;

	if (bShouldDrawDebug)
	{
#if WITH_EDITOR
		World->DebugDrawTraceTag = TraceTag;
#endif
	}
	World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Camera, CQP);

	CheckForInteractables(HitResult);
}

void ASG_Character::CheckForInteractables(FHitResult HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	UPrimitiveComponent* HitComponent = HitResult.GetComponent();
	IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(HitActor);

	if (FocusedInteractableObject == HitActor)
	{
		return;
	}
	if (InteractableActor == nullptr)
	{
		if (FocusedInteractableObject == nullptr)
		{
			return;
		}
		UActorComponent* InteractableMeshComponent = Cast<AActor>(FocusedInteractableObject)->GetComponentByClass(UMeshComponent::StaticClass());
		if (Cast<UMeshComponent>(InteractableMeshComponent) && Cast<APlayerController>(GetController()) != nullptr)
		{
			Cast<UMeshComponent>(InteractableMeshComponent)->SetOverlayMaterial(nullptr);
		}

		InteractIndicationTextDelegate.Broadcast(FText::FromString(""));
		Server_SetFocusedInteractable(nullptr);
		return;
	}
	if (!InteractableActor->bCanInteract(GetController()))
	{
		return;
	}
	if (FocusedInteractableObject != nullptr)
	{
		UActorComponent* InteractableMeshComponent = Cast<AActor>(FocusedInteractableObject)->GetComponentByClass(UMeshComponent::StaticClass());
		if (Cast<UMeshComponent>(InteractableMeshComponent) && Cast<APlayerController>(GetController()) != nullptr)
		{
			Cast<UMeshComponent>(InteractableMeshComponent)->SetOverlayMaterial(nullptr);
		}
	}

	UMeshComponent* HitMeshComponent = Cast<UMeshComponent>(HitActor->GetComponentByClass(UMeshComponent::StaticClass()));
	if (HitMeshComponent != nullptr && Cast<APlayerController>(GetController()) != nullptr)
	{
		HitMeshComponent->SetOverlayMaterial(InteractableOverlayMaterial);
	}

	InteractIndicationTextDelegate.Broadcast(FText::FromString("E " + InteractableActor->GetInteractableName().ToString()));
	Server_SetFocusedInteractable(HitActor);
}

void ASG_Character::BeginPlay()
{
	Super::BeginPlay();

	CurrentInteractionRange = InteractionRange;
	
	GetCharacterMovement()->bOrientRotationToMovement = !bUsesEightDirectionalMovement;

	if (IsValid(GetAbilitySystemComponent()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::BeginPlay IsValid(GetAbilitySystemComponent()) == false"))
		return;
	}
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);

	if (IsValid(CameraViewResetCurve))
	{
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindUFunction(this, FName("UpdateCamera"));

		CameraViewResetTimeline.AddInterpFloat(CameraViewResetCurve, ProgressUpdate);
		CameraViewResetTimeline.SetLooping(false);
	}
}

void ASG_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraViewResetTimeline.TickTimeline(DeltaTime);

	if (IsLocallyControlled() == false)
	{
		return;
	}
	EmitInteractionChecker();
}

void ASG_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (USG_EnhancedInputComponent* SG_EnhancedInputComponent = Cast<USG_EnhancedInputComponent>(PlayerInputComponent)) 
	{
		SG_EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASG_Character::Move);

		SG_EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASG_Character::Look);

		SG_EnhancedInputComponent->BindAction(CameraModeToggleAction, ETriggerEvent::Triggered, this, &ASG_Character::ToggleFirstPerson);

		SG_EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &ASG_Character::StartTarget);
		SG_EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Completed, this, &ASG_Character::StopTarget);

		if (IsValid(AbilityInputConfig) == false)
		{
			UE_LOG(LogSGCharacter, Error, TEXT("ASG_Character::SetupPlayerInputComponent IsValid(AbilityInputConfig) == false"))
			return;
		}
		SG_EnhancedInputComponent->BindAbilityActions(AbilityInputConfig, this, &ASG_Character::AbilityInputPressed, &ASG_Character::AbilityInputReleased);
	}
	else
	{
		UE_LOG(LogSGCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASG_Character::ToggleFirstPerson()
{
	bIsFirstPerson = !bIsFirstPerson;

	if (bIsFirstPerson)
	{
		CameraBoom->TargetArmLength = 0.f;
		CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("head"));
		FirstPersonCamera->SetRelativeLocation(FVector::Zero());
		CurrentInteractionRange = InteractionRange;
	}
	else
	{
		CameraBoom->TargetArmLength = CameraDistanceFromPlayer;
		CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ThirdPersonCameraSocket"));
		FirstPersonCamera->SetRelativeLocation(FVector(0, 30, 0));
		CurrentInteractionRange = InteractionRange + CameraDistanceFromPlayer;
	}
}

void ASG_Character::Move(const FInputActionValue& Value)
{
	if (bCanMove == false)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (bIsCameraMovementConstrained)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (IsValid(PlayerController))
		{
			PlayerController->PlayerCameraManager->ViewYawMin = GetActorRotation().Yaw - 80;
			PlayerController->PlayerCameraManager->ViewYawMax = GetActorRotation().Yaw + 80;
		}
	}
	
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
	
}

void ASG_Character::Look(const FInputActionValue& Value)
{
	if (GetWorldTimerManager().IsTimerActive(CameraRotationTimer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Nuh uh"))
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>(); 

	if (Controller == nullptr)
	{
		return;
	}
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);

	SetRepControlRotation(GetControlRotation());

}

void ASG_Character::AbilityInputPressed(FGameplayTag InputTag)
{
	if (IsValid(GetAbilitySystemComponent()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::AbilityInputPressed IsValid(GetAbilitySystemComponent()) == false"));
		return;
	}
	AbilitySystemComponent->AbilityInputPressed(InputTag);
}

void ASG_Character::AbilityInputReleased(FGameplayTag InputTag)
{
	if (IsValid(GetAbilitySystemComponent()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::AbilityInputReleased IsValid(GetAbilitySystemComponent()) == false"));
		return;
	}
	AbilitySystemComponent->AbilityInputReleased(InputTag);
}

void ASG_Character::Server_SetRepControlRotation_Implementation(FRotator InControlRotation)
{
	SetRepControlRotation(InControlRotation);
}

void ASG_Character::SetRepControlRotation(FRotator InControlRotation)
{
	if (HasAuthority() == false)
	{
		Server_SetRepControlRotation(InControlRotation);
	}
	ControlRotation = InControlRotation;
}

void ASG_Character::Cliet_UnbindOverlayDelegates_Implementation()
{
	UnbindOverlayDelegates();
}

void ASG_Character::UnbindOverlayDelegates()
{
	InteractIndicationTextDelegate.Broadcast(FText::FromString(""));
	InteractIndicationTextDelegate.Clear();
}

void ASG_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (IsValid(PlayerController))
	{
		if (bIsCameraMovementConstrained)
		{
			PlayerController->PlayerCameraManager->ViewYawMin = GetActorRotation().Yaw - 80;
			PlayerController->PlayerCameraManager->ViewYawMax = GetActorRotation().Yaw + 80;
		}
	}

	if (bAlreadySetUp) return;

	InitCharacterDefaults();
}

void ASG_Character::InitCharacterDefaults()
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::InitCharacterDefaults IsValid(AbilitySystemComponent) == false"));
		return;
	}
	
	for (TSubclassOf<USG_GameplayAbility> Ability : DefaultAbilities)
	{
		AbilitySystemComponent->AddAbility(Ability, 1, this);
	}
	
	for (TSubclassOf<USG_GameplayAbility> Ability : DefaultPassiveAbilities)
	{
		AbilitySystemComponent->AddPassiveAbility(Ability, 1, this);
	}

	AbilitySystemComponent->AddEffectFromClass(DefaultAttributeEffect, this, this, this);
	AbilitySystemComponent->SetStartingAttributeValues();

	bAlreadySetUp = true;
}

UAbilitySystemComponent* ASG_Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UObject* ASG_Character::GetFocusedInteractableObject()
{
	return FocusedInteractableObject;
}

FVector ASG_Character::GetPlayerLookAtLocation(float DistanceFromPlayer)
{
	return (GetActorLocation() + GetFirstPersonCamera()->GetRelativeLocation()) + GetFirstPersonCamera()->GetForwardVector() * DistanceFromPlayer;
}

FRotator ASG_Character::GetRepControlRotation()
{
	return ControlRotation;
}

void ASG_Character::ResetCameraView()
{
	if (IsLocallyControlled() == false)
	{
		return;
	}
	CameraViewResetTimeline.PlayFromStart();
}

void ASG_Character::StartTarget()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::StartTarget IsValid(World) == false"));
		return;
	}

	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = (FirstPersonCamera->GetForwardVector() * TargetingDistance) + TraceStart;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitResults;

	if (UKismetSystemLibrary::SphereTraceMultiForObjects(World, TraceStart, TraceEnd, TargetingTraceRadius, ObjectTypes, false,
		ActorsToIgnore, EDrawDebugTrace::None, HitResults, true) == false)
	{
		ResetCameraView();
		return;
	}

	for (FHitResult HitResult : HitResults)
	{
		if (IsValid(HitResult.GetActor()))
		{
			PointCameraTowardsActor(World, HitResult.GetActor());
			return;
		}
	}
}

void ASG_Character::StopTarget()
{
	GetWorldTimerManager().ClearTimer(CameraRotationTimer);
}

void ASG_Character::PointCameraTowardsActor(UWorld* World, AActor* TargetActor)
{
	if (IsValid(GetController()) == false)
	{
		return;
	}
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::PointCameraTowardsActor IsValid(World) == false"));
		return;
	}
	if (IsValid(TargetActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::PointCameraTowardsActor IsValid(TargetActor) == false"));
		StopTarget();
		return;
	}

	if (UKismetMathLibrary::Vector_Distance(GetActorLocation(), TargetActor->GetActorLocation()) > TargetingDistance)
	{
		StopTarget();
	}

	FVector NormalizedLocation = TargetActor->GetActorLocation() - GetMesh()->GetComponentLocation();
	UKismetMathLibrary::Vector_Normalize(NormalizedLocation);

	float DotProduct = FVector::DotProduct(NormalizedLocation, GetMesh()->GetForwardVector());
	if (FMath::Abs(DotProduct) > 0.1f)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(FirstPersonCamera->GetComponentLocation(), TargetActor->GetActorLocation());
		FRotator InterpRotation = UKismetMathLibrary::RInterpTo(GetControlRotation(), LookAtRotation, World->DeltaTimeSeconds, CameraInterpSpeed);

		GetController()->SetControlRotation(InterpRotation);
		SetActorRotation(FRotator(GetActorRotation().Pitch, InterpRotation.Yaw, GetActorRotation().Roll));
		SetRepControlRotation(GetControlRotation());
	}
	
	FTimerDelegate CameraRotationTimerDelegate;

	CameraRotationTimerDelegate.BindUFunction(this, FName("PointCameraTowardsActor"), World, TargetActor);
	GetWorldTimerManager().SetTimer(CameraRotationTimer, CameraRotationTimerDelegate, 0.01f, false);
}

void ASG_Character::SetMovementEnabled(bool NewValue)
{
	bCanMove = NewValue ? true : false;
}

bool ASG_Character::IsMovementEnabled() const
{
	return bCanMove;
}

void ASG_Character::DisablePlayerInput(float Duration)
{
	if (IsLocallyControlled() == false || bIsInputEnabled == false)
	{
		return;
	}

	if (IsValid(Cast<APlayerController>(GetController())))
	{
		DisableInput(Cast<APlayerController>(GetController()));
	}
	else if (IsValid(Cast<ASG_AIController>(GetController())))
	{
		Cast<ASG_AIController>(GetController())->StopLogic();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not disable input!"), *GetName());
		return;
	}

	bIsInputEnabled = false;

	if (Duration > 0.f)
	{
		FTimerHandle EnableInputTimer;
		GetWorldTimerManager().SetTimer(EnableInputTimer, this, &ASG_Character::EnablePlayerInput, Duration, false);
	}
	UE_LOG(LogTemp, Log, TEXT("%s: Disabling input for %.3f seconds"), *GetName(), Duration);
}

void ASG_Character::EnablePlayerInput()
{
	if (IsLocallyControlled() == false || bIsInputEnabled)
	{
		return;
	}

	if (Cast<APlayerController>(GetController()))
	{
		EnableInput(Cast<APlayerController>(GetController()));
	}
	else if (IsValid(Cast<ASG_AIController>(GetController())))
	{
		Cast<ASG_AIController>(GetController())->StartLogic();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not enable input!"), *GetName());
		return;
	}

	bIsInputEnabled = true;
	UE_LOG(LogTemp, Log, TEXT("%s: Enabling input"), *GetName());
}

bool ASG_Character::IsPlayerInputEnabled()
{
	return bIsInputEnabled;
}

bool ASG_Character::IsMoving()
{
	return (GetVelocity().Length() != 0);
}

bool ASG_Character::IsMirrored()
{
	return bIsMirrored;
}

void ASG_Character::SetIsMirrored(bool NewValue)
{
	if (HasAuthority() == false)
	{
		Server_SetIsMirrored(NewValue);
		return;
	}

	bIsMirrored = NewValue;
}

void ASG_Character::Server_SetIsMirrored_Implementation(bool NewValue)
{
	SetIsMirrored(NewValue);
}

void ASG_Character::SendGameplayEventToSelf(FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly)
{
	if (HasAuthority() == false)
	{
		Server_SendGameplayEventToSelf(EventTag, EventData, bServerOnly);
		if(bServerOnly) return;
	}

	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void ASG_Character::Server_SendGameplayEventToSelf_Implementation(FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly)
{
	SendGameplayEventToSelf(EventTag, EventData, bServerOnly);
}

