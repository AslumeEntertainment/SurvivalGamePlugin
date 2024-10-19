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


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ASG_Character::ASG_Character()
{
	PrimaryActorTick.bCanEverTick = true;

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
}

void ASG_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	//--------------------------------this must not be here-----------------------------
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
	//------------------------------------------------------------------------------------
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

void ASG_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

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

	InitCharacterDefaults();
	BindCallbacksToDependencies();
}

void ASG_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void ASG_Character::InitCharacterDefaults()
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::InitCharacterDefaults IsValid(AbilitySystemComponent) == false"));
		return;
	}
	/* There might be a better way to do this */
	TMap<TSubclassOf<USG_GameplayAbility>, int> DefaultAbilitiesMap;
	for (TSubclassOf<USG_GameplayAbility> Ability : DefaultAbilities)
	{
		DefaultAbilitiesMap.Add(Ability, 1);
	}
	
	TMap<TSubclassOf<USG_GameplayAbility>, int> DefaultPassiveAbilitiesMap;
	for (TSubclassOf<USG_GameplayAbility> Ability : DefaultPassiveAbilities)
	{
		DefaultPassiveAbilitiesMap.Add(Ability, 1);
	}

	AbilitySystemComponent->AddAbilitiesFromMap(DefaultAbilitiesMap, this);
	AbilitySystemComponent->AddPassiveAbilitiesFromMap(DefaultPassiveAbilitiesMap, this);
	AbilitySystemComponent->AddEffectFromClass(DefaultAttributeEffect, this);
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

void ASG_Character::DisablePlayerInput(float Duration)
{
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
		GetWorldTimerManager().SetTimer(EnableInputTimer, this, &ASG_Character::EnablePlayerInput, Duration, false, 0);
	}
}

void ASG_Character::EnablePlayerInput()
{
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

	InputSystem->AddMappingContext(DefaultMappingContext, 0);
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

	/*USG_AnimInstance* AnimInst = Cast<USG_AnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInst) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::SetIsMirrored IsValid(AnimInst) == false"))
		return;
	}
	AnimInst->bIsMirrored = NewValue;*/
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
		Server_SendGameplayEventToSelf(EventTag, EventData);
		if(bServerOnly) return;
	}

	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void ASG_Character::Server_SendGameplayEventToSelf_Implementation(FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly)
{
	SendGameplayEventToSelf(EventTag, EventData);
}

