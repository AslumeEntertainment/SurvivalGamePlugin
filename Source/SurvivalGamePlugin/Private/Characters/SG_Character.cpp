// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SG_Character.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
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
#include "UI/HUD/InGameHUD.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

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
	CameraBoom->SetupAttachment(GetMesh(), FName(TEXT("head")));
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
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::UpdateCamera IsValid(PlayerCont) == false"));
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
		World->DebugDrawTraceTag = TraceTag;
	}
	World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_GameTraceChannel1, CQP);

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
		if (Cast<UMeshComponent>(InteractableMeshComponent))
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
		if (Cast<UMeshComponent>(InteractableMeshComponent))
		{
			Cast<UMeshComponent>(InteractableMeshComponent)->SetOverlayMaterial(nullptr);
		}
	}

	UMeshComponent* HitMeshComponent = Cast<UMeshComponent>(HitActor->GetComponentByClass(UMeshComponent::StaticClass()));
	if (HitMeshComponent != nullptr)
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

	/*if (IsLocallyControlled() && HasAuthority())
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (IsValid(PlayerController) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("ASG_Character::BeginPlay IsValid(PlayerController) == false"));
			return;
		}
		AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());
		if (IsValid(HUD) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("ASG_Character::BeginPlay IsValid(HUD) == false"));
			return;
		}
		HUD->SetInteractionOverlayBindings(this);
	}*/
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

		if (IsValid(AbilityInputConfig) == false)
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("ASG_Character::SetupPlayerInputComponent IsValid(AbilityInputConfig) == false"))
			return;
		}
		SG_EnhancedInputComponent->BindAbilityActions(AbilityInputConfig, this, &ASG_Character::AbilityInputPressed, &ASG_Character::AbilityInputReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (bIsCameraMovementConstrained)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (!IsValid(PlayerController))
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("ASG_Character::Move IsValid(PlayerController) == false"));
			return;
		}
		PlayerController->PlayerCameraManager->ViewYawMin = GetActorRotation().Yaw - 80;
		PlayerController->PlayerCameraManager->ViewYawMax = GetActorRotation().Yaw + 80;
	}
	
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
	
}

void ASG_Character::Look(const FInputActionValue& Value)
{
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

void ASG_Character::DisablePlayerInput(float Duration)
{
	if (IsLocallyControlled() == false)
	{
		return;
	}
	APlayerController* PlayerCont = Cast<APlayerController>(GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::DisablePlayerInput IsValid(PlayerCont) == false"));
		return;
	}
	UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerCont->GetLocalPlayer());
	if (IsValid(InputSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::DisablePlayerInput IsValid(InputSystem) == false"));
		return;
	}

	InputSystem->ClearAllMappings();
	if (Duration > 0.f)
	{
		FTimerHandle EnableInputTimer;
		GetWorldTimerManager().SetTimer(EnableInputTimer, this, &ASG_Character::EnablePlayerInput, Duration, false);
	}

	UE_LOG(LogTemp, Log, TEXT("ASG_Character::DisablePlayerInput - %s"), *GetName());
}

void ASG_Character::EnablePlayerInput()
{
	if (IsLocallyControlled() == false)
	{
		return;
	}
	APlayerController* PlayerCont = Cast<APlayerController>(GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::EnablePlayerInput IsValid(PlayerCont) == false"));
		return;
	}
	UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerCont->GetLocalPlayer());
	if (IsValid(InputSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::EnablePlayerInput IsValid(InputSystem) == false"));
		return;
	}

	InputSystem->AddMappingContext(DefaultMappingContext, 0);
	UE_LOG(LogTemp, Log, TEXT("ASG_Character::EnablePlayerInput - %s"), *GetName());
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

