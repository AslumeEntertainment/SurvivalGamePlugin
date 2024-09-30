// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SG_Character.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Interfaces/InteractableInterface.h"
#include "Interfaces/AbilitySystemControllerInterface.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_GameplayEffectContext.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

// Sets default values
ASG_Character::ASG_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	GetCharacterMovement()->JumpZVelocity = 700.f;
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
}

void ASG_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ASG_Character, FocusedInteractableObject, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ASG_Character, ControlRotation, COND_None, REPNOTIFY_Always);
}

void ASG_Character::Server_SetFocusedInteractable_Implementation(UObject* InFocusedInteractable)
{
	FocusedInteractableObject = InFocusedInteractable;
}

void ASG_Character::EmitInteractionChecker()
{
	FVector StartTrace = GetFirstPersonCamera()->GetComponentLocation();
	FVector EndTrace = GetFirstPersonCamera()->GetForwardVector() * InteractionRange + StartTrace;

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
}

void ASG_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
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

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASG_Character::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASG_Character::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASG_Character::AttemptInteraction);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASG_Character::Server_SetRepControlRotation_Implementation(FRotator InControlRotation)
{
	ControlRotation = InControlRotation;
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

	Server_SetRepControlRotation(GetControlRotation());
}

void ASG_Character::AttemptInteraction()
{
	AController* Cont = GetController();
	if (IsValid(Cont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::AttemptInteraction IsValid(Cont) == false"));
		return;
	}

	IAbilitySystemControllerInterface* ASCI = Cast<IAbilitySystemControllerInterface>(Cont);
	if (ASCI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::AttemptInteraction ASCI == nullptr"));
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("ASG_Character::AttemptInteraction Cont and ASC == true"));

	if (FocusedInteractableObject == nullptr)
	{
		if (bShouldDrawDebug == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("COULD NOT INTERACT WITH EMPTY FOCUS"));
		}
		return;
	}

	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::Interact);
}

UObject* ASG_Character::GetFocusedInteractableObject()
{
	return FocusedInteractableObject;
}

FVector ASG_Character::GetPlayerLookAtLocation(float DistanceFromPlayer)
{
	return GetActorLocation() + GetFirstPersonCamera()->GetForwardVector() * DistanceFromPlayer;
}

FRotator ASG_Character::GetRepControlRotation()
{
	return ControlRotation;
}

void ASG_Character::InitializeAttributes()
{
	IAbilitySystemControllerInterface* ASCI = Cast<IAbilitySystemControllerInterface>(GetController());
	if (ASCI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::InitializeAttributes ASCI == nullptr"));
		return;
	}

	ASCI->AddEffect(DefaultAttributeEffect);
	//UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent();

	//if (ASC && DefaultAttributeEffect)
	//{
	//	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	//	EffectContext.AddSourceObject(this);

	//	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

	//	if (SpecHandle.IsValid())
	//	{
	//		FActiveGameplayEffectHandle GEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	//	}

	//	//ASC->ApplyDefaultEffectsToSelf();
	//}
}

void ASG_Character::GiveAbilities()
{
	IAbilitySystemControllerInterface* ASCI = Cast<IAbilitySystemControllerInterface>(GetController());
	if (ASCI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::GiveAbilities ASCI == nullptr"));
		return;
	}

	if (HasAuthority())
	{
		for (TSubclassOf<USG_GameplayAbility>& StartupAbility : DefaultAbilities)
		{
			ASCI->AddAbility(StartupAbility);
		}
	}
}

void ASG_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UE_LOG(LogTemp, Warning, TEXT("ASG_Character::PossessedBy"));

	IAbilitySystemControllerInterface* ASCI = Cast<IAbilitySystemControllerInterface>(GetController());
	if (ASCI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::PossessedBy ASCI == nullptr"));
		return;
	}

	ASCI->InitASC(this);
	GiveAbilities();

	/*UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent();
	if (ASC && InputComponent)
	{
		FTopLevelAssetPath InputEnumPath = FTopLevelAssetPath(TEXT("/Script/SurvivalGamePlugin"), TEXT("EGameplayAbilityInputID"));
		const FGameplayAbilityInputBinds Binds(FString("Confirm"), FString("Cancel"), InputEnumPath, static_cast<int32>(EGameplayAbilityInputID::Confirm), static_cast<int32>(EGameplayAbilityInputID::Cancel));
		ASC->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}


	ASC->InitAbilityActorInfo(ASCI->GetAbilitySystemComponentOwner(), this);*/
}

//void ASG_Character::Client_PossessedBy_Implementation(AController* NewController)
//{
//	if (HasAuthority())
//	{
//		return;
//	}
//
//	UE_LOG(LogTemp, Warning, TEXT("ASG_Character::Client_PossessedBy_Implementation"));
//
//	IAbilitySystemControllerInterface* ASCI = Cast<IAbilitySystemControllerInterface>(GetController());
//	if (ASCI == nullptr)
//	{
//		UE_LOG(LogTemp, Error, TEXT("ASG_Character::Client_PossessedBy_Implementation ASCI == nullptr"));
//		return;
//	}
//
//	ASCI->InitASC(this);
//}

//void ASG_Character::UnPossessed()
//{
//	Super::UnPossessed();
//}

void ASG_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UE_LOG(LogTemp, Warning, TEXT("ASG_Character::OnRep_PlayerState"));

	if (GetPlayerState() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::OnRep_PlayerState GetPlayerState() == nullptr, ebi si maikata"));
		return;
	}

	AController* Cont = GetController();
	if (IsValid(Cont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::OnRep_PlayerState IsValid(Cont) == false"));

		FTimerHandle FunctionResetTimer;
		return;
	}

	IAbilitySystemControllerInterface* ASCI = Cast<IAbilitySystemControllerInterface>(Cont);
	if (ASCI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::OnRep_PlayerState ASCI == nullptr"));
		return;
	}

	ASCI->InitASC(this);
	//InitializeAttributes();
}

void ASG_Character::SendLocalInputToAbilitySystemComponent(bool bIsPressed, const EGameplayAbilityInputID AbilityInputID)
{
	/*if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("ASG_Character::SendLocalInputToAbilitySystemComponent IsValid(AbilitySystemComponent) == false"));
		return;
	}

	if (bIsPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}*/
}

