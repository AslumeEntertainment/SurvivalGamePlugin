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
#include "Net/UnrealNetwork.h"

#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_AttributeSet.h"
#include "GAS/SG_GameplayAbility.h"

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
	GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkingSpeed;
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
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USG_AttributeSet>(TEXT("AttributeSet"));
}

void ASG_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ASG_Character, ControlRotation, COND_None, REPNOTIFY_Always);
}

// Called when the game starts or when spawned
void ASG_Character::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAttributes();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("ASG_Character::BeginPlay IsValid(PlayerController) == false"));
		return;
	}

	if (bIsCamersMovementConstrained)
	{
		PlayerController->PlayerCameraManager->ViewYawMin = -80.f;
		PlayerController->PlayerCameraManager->ViewYawMax = 80.f;
		PlayerController->PlayerCameraManager->ViewPitchMin = -55.f;
		PlayerController->PlayerCameraManager->ViewPitchMax = 55.f;
	}
}

// Called every frame
void ASG_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASG_Character::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASG_Character::Look);

		if (AbilitySystemComponent && InputComponent)
		{
			FTopLevelAssetPath InputEnumPath = FTopLevelAssetPath(TEXT("/Script/SurvivalGamePlugin"), TEXT("EGameplayAbilityInputID"));
			const FGameplayAbilityInputBinds Binds(FString("Confirm"), FString("Cancel"), InputEnumPath, static_cast<int32>(EGameplayAbilityInputID::Confirm), static_cast<int32>(EGameplayAbilityInputID::Cancel));
			AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASG_Character::ServerSetRepControlRotation_Implementation(FRotator InControlRotation)
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

	if (bIsCamersMovementConstrained)
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

	ServerSetRepControlRotation(GetControlRotation());
}

void ASG_Character::MultiplyMaxWalkingSpeed(float SpeedMultiplier)
{
	GetCharacterMovement()->MaxWalkSpeed *= SpeedMultiplier;
}

void ASG_Character::ResetWalkingSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkingSpeed;
}

void ASG_Character::OnHitNotifyRecieved_Implementation(FVector DamageOrigin)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::OnHitNotifyRecieved_Implementation IsValid(World) == false"));
		return;
	}
	
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	UKismetSystemLibrary::DrawDebugSphere(World, DamageOrigin, 50.f, 12, FLinearColor::Green, 2.f, 1.f);

	if (!UKismetSystemLibrary::SphereOverlapActors(World, DamageOrigin, 50.f, ObjectTypes, nullptr, ActorsToIgnore, OutActors))
	{
		UE_LOG(LogTemp, Warning, TEXT("ASG_Character::OnHitNotifyRecieved_Implementation - Nothing was hit"));
		return;
	}
	for (AActor* OutActor : OutActors)
	{
		IHittableInterface* HittableActor = Cast<IHittableInterface>(OutActor);
		if (HittableActor == nullptr)
		{
			continue;
		}
		if (HittableActor->bCanBeHit() == false)
		{
			continue;
		}
		HittableActor->OnHit(this);
	}
}

void ASG_Character::OnHit(ASG_Character* InstigatingPlayer)
{
	FGameplayEventData EventData;
	EventData.Instigator = InstigatingPlayer;
	EventData.Target = this;
	EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(this);
	
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Combat.Hit"));
	if (!EventTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_Character::OnHit EventTag.IsValid() == false"));
		return;
	}
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstigatingPlayer, EventTag, EventData);
}

bool ASG_Character::bCanBeHit()
{
	return IsAlive();
}

void ASG_Character::Die()
{
	bIsAlive = false;

	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetHealthAttribute(), 0);
	UnPossessed();
}

FVector ASG_Character::GetPlayerLookAtLocation(float DistanceFromPlayer)
{
	return GetActorLocation() + GetFirstPersonCamera()->GetForwardVector() * DistanceFromPlayer;
}

FRotator ASG_Character::GetRepControlRotation()
{
	return ControlRotation;
}

float ASG_Character::GetBaseDamage()
{
	return BaseDamage;
}

bool ASG_Character::IsAlive()
{
	return bIsAlive;
}

UAbilitySystemComponent* ASG_Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASG_Character::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetDamageRightAttribute(), BaseDamage);
		AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetDamageLeftAttribute(), BaseDamage);

		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ASG_Character::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (TSubclassOf<USG_GameplayAbility>& StartupAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		}
	}
}

FGameplayAbilitySpecHandle ASG_Character::AddAbility(TSubclassOf<class USG_GameplayAbility> AbilityClass)
{
	if (!HasAuthority())
	{
		return FGameplayAbilitySpecHandle();
	}

	if (!IsValid(AbilityClass))
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("ASG_Character::AddAbility_Implementation IsValid(AbilityClass) == false"));
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, static_cast<int32>(AbilityClass.GetDefaultObject()->AbilityInputID), this));
	UE_LOG(LogTemplateCharacter, Warning, TEXT("Ability given!"));
	return AbilitySpecHandle;
}

void ASG_Character::RemoveAbility(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle)
{
	if (!HasAuthority())
	{
		return;
	}

	AbilitySystemComponent->ClearAbility(RemovedAbilitySpecHandle);
	UE_LOG(LogTemplateCharacter, Warning, TEXT("Ability Removed!"));
}

void ASG_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	GiveAbilities();
}

void ASG_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (AbilitySystemComponent && InputComponent)
	{
		FTopLevelAssetPath InputEnumPath = FTopLevelAssetPath(TEXT("/Script/SurvivalGamePlugin"), TEXT("EGameplayAbilityInputID"));
		const FGameplayAbilityInputBinds Binds(FString("Confirm"), FString("Cancel"), InputEnumPath, static_cast<int32>(EGameplayAbilityInputID::Confirm), static_cast<int32>(EGameplayAbilityInputID::Cancel));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void ASG_Character::SendLocalInputToAbilitySystemComponent(bool bIsPressed, const EGameplayAbilityInputID AbilityInputID)
{
	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("ASG_Character::SendLocalInputToAbilitySystemComponent IsValid(AbilitySystemComponent) == false"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Sending ability input ID to Ability System Component"));

	if (bIsPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}
}

