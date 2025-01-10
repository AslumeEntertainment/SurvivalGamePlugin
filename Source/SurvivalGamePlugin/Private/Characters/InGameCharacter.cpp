// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/InGameCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

#include "Interactables/Items/DamageDealingItem.h"
#include "Components/HittableComponent.h"
#include "Components/DamageDealingComponent.h"
#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "UI/HUD/InGameHUD.h"


AInGameCharacter::AInGameCharacter()
{
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

	LeftHandDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("LeftHandDamageOrigin"));
	LeftHandDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("hand_l"));

	RightHandDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("RightHandDamageOrigin"));
	RightHandDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("hand_r"));

	HittableComponent = CreateDefaultSubobject<UHittableComponent>(TEXT("PassiveDamageDealingComponent"));
	HittableComponent->SetupAttachment(GetRootComponent());
	
	OnDestroyed.AddDynamic(this, &AInGameCharacter::OnInGameCharacterDestroyed);
}

void AInGameCharacter::NetMulticast_SetHittableCompHalfHeight_Implementation(float NewValue)
{
	HittableComponent->SetCapsuleHalfHeight(NewValue);
}

void AInGameCharacter::OnInGameCharacterDestroyed(AActor* RefToSelf)
{
	TArray<AItemBase*> Equipment = EquipmentComponent->GetAllEquipedItems();
	for (AItemBase* Item : Equipment)
	{
		Item->Destroy();
	}

	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (IsValid(PlayerController) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BeginPlay IsValid(PlayerController) == false"));
			return;
		}
		AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());
		if (IsValid(HUD) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BeginPlay IsValid(HUD) == false"));
			return;
		}
		HUD->SetInteractionOverlayBindings(this);
	}
}

void AInGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AInGameCharacter, AttackSpeed, COND_None, REPNOTIFY_Always);
}

void AInGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	OldHittableCompHalfHeight = HittableComponent->GetUnscaledCapsuleHalfHeight();

	if (bIsRightHanded == false)
	{
		EquipmentComponent->SwapMainAndSecondaryEquipmentSlots();
	}

	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (IsValid(PlayerController) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BeginPlay IsValid(PlayerController) == false"));
			return;
		}
		AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());
		if (IsValid(HUD) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BeginPlay IsValid(HUD) == false"));
			return;
		}
		OnCharacterDeath.AddDynamic(HUD, &AInGameHUD::RemoveInteractionOverlayWidget);
	}
}

void AInGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		Client_OnUpdatePlayerCurrentSpeed(GetVelocity().Length());
	}
}

void AInGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BindCallbacksToDependencies();
}

void AInGameCharacter::Client_OnUpdatePlayerHealth_Implementation(float CurrentHealth, float MaxHealth)
{
	OnHealthPercentUpdated.Broadcast(CurrentHealth / MaxHealth);
}

void AInGameCharacter::Client_OnUpdatePlayerMaxHealth_Implementation(float MaxHealth)
{
	OnMaxHealthUpdated.Broadcast(MaxHealth);
}

void AInGameCharacter::Client_OnUpdatePlayerHunger_Implementation(float CurrentHunger, float MaxHunger)
{
	OnHungerPercentUpdated.Broadcast(CurrentHunger / MaxHunger);
}

void AInGameCharacter::Client_OnUpdatePlayerMaxHunger_Implementation(float MaxHunger)
{
	OnMaxHungerUpdated.Broadcast(MaxHunger);
}

void AInGameCharacter::Client_OnUpdatePlayerEnergy_Implementation(float CurrentEnergy, float MaxEnergy)
{
	OnEnergyPercentUpdated.Broadcast(CurrentEnergy / MaxEnergy);
}

void AInGameCharacter::Client_OnUpdatePlayerMaxEnergy_Implementation(float MaxEnergy)
{
	OnMaxEnergyUpdated.Broadcast(MaxEnergy);
}

void AInGameCharacter::Client_OnUpdatePlayerCurrentSpeed_Implementation(float Velocity)
{
	OnCurrentSpeedUpdated.Broadcast(Velocity);
}

void AInGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Triggered, this, &AInGameCharacter::MainHandAction);
		EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Triggered, this, &AInGameCharacter::SecondaryHandAction);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AInGameCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	FVector NormalVelocity = UKismetMathLibrary::Normal(GetVelocity());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("NormalVelocity.Z - %.2f"), NormalVelocity.Z));
	if (NormalVelocity.Z > -0.45)
	{
		return;
	}

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag("CommonEvents.ReceiveDamage.Fall");
	SendGameplayEventToSelf(EventTag, FGameplayEventData());
}

void AInGameCharacter::MainHandAction()
{
	FGameplayTag EventTag;
	FGameplayEventData EventData;

	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
	{
		if (EquipmentComponent->HasEquipmentSlotWithTag(EquipmentComponent->GetMixedEquipmentSlotTag()))
		{
			EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Mixed");
		}
		else
		{
			EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Main");
		}
	}
	else
	{
		FString ActionSuffix = IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMainEquipmentSlotTag())) || 
				IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMixedEquipmentSlotTag())) ? ".Item" : ".Attack";
		FString MirroringSuffix = bIsRightHanded ? ".Right" : ".Left";
		EventTag = FGameplayTag::RequestGameplayTag(FName("PlayerAction.Main" + ActionSuffix + MirroringSuffix));
	}
	UE_LOG(LogTemp, Warning, TEXT("MainHandAction - %s"), *EventTag.ToString());

	SendGameplayEventToSelf(EventTag, EventData);
}

void AInGameCharacter::SecondaryHandAction()
{
	FGameplayTag EventTag;
	FGameplayEventData EventData;

	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
	{
		if (EquipmentComponent->HasEquipmentSlotWithTag(EquipmentComponent->GetMixedEquipmentSlotTag()))
		{
			EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Mixed");
		}
		else
		{
			EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Secondary");
		}
	}
	else
	{
		FString ActionSuffix = IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetSecondaryEquipmentSlotTag())) ||
			IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMixedEquipmentSlotTag())) ? ".Item" : ".Attack";
		FString MirroringSuffix = bIsRightHanded ? ".Left" : ".Right";
		EventTag = FGameplayTag::RequestGameplayTag(FName("PlayerAction.Secondary" + ActionSuffix + MirroringSuffix));
	}
	UE_LOG(LogTemp, Warning, TEXT("SecondaryHandAction - %s"), *EventTag.ToString());

	SendGameplayEventToSelf(EventTag, EventData);
}

void AInGameCharacter::BindCallbacksToDependencies()
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BindCallbacksToDependencies IsValid(AbilitySystemComponent) == false"));
		return;
	}
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BindCallbacksToDependencies IsValid(AttributeSet) == false"));
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerHealth(Data.NewValue, AttributeSet->GetMaxHealth());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerMaxHealth(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHungerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerHunger(Data.NewValue, AttributeSet->GetMaxHunger());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHungerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerMaxHunger(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerEnergy(Data.NewValue, AttributeSet->GetMaxEnergy());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerMaxEnergy(Data.NewValue);
		}
	);
}

void AInGameCharacter::BroadcastInitialValues()
{
	if (HasAuthority() == false)
	{
		FTimerHandle BroadcastInitialValuesTimerHandle;
		GetWorldTimerManager().SetTimer(BroadcastInitialValuesTimerHandle, this, &AInGameCharacter::Client_BroadcastInitialValues, 0.5f, false);
	}
	
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BroadcastInitialValues IsValid(AttributeSet) == false"));
		return;
	}

	Client_OnUpdatePlayerHealth(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
	Client_OnUpdatePlayerMaxHealth(AttributeSet->GetMaxHealth());
	Client_OnUpdatePlayerHunger(AttributeSet->GetHunger(), AttributeSet->GetMaxHunger());
	Client_OnUpdatePlayerMaxHunger(AttributeSet->GetMaxHunger());
	Client_OnUpdatePlayerEnergy(AttributeSet->GetEnergy(), AttributeSet->GetMaxEnergy());
	Client_OnUpdatePlayerMaxEnergy(AttributeSet->GetMaxEnergy());
}

void AInGameCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
	
	if (HasAuthority())
	{
		NetMulticast_SetHittableCompHalfHeight(HittableCompOnCrouchedHalfHeight);
	}
}

void AInGameCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
	
	if (HasAuthority())
	{
		NetMulticast_SetHittableCompHalfHeight(OldHittableCompHalfHeight);
	}
}

void AInGameCharacter::Client_BroadcastInitialValues_Implementation()
{
	BroadcastInitialValues();
}

void AInGameCharacter::SetAttackSpeed(float NewValue)
{
	AttackSpeed = NewValue;
}

float AInGameCharacter::GetAttackSpeed() const
{
	return AttackSpeed;
}

bool AInGameCharacter::IsRightHanded()
{
	return bIsRightHanded;
}

void AInGameCharacter::SetIsRightHanded(bool NewValue)
{
	bIsRightHanded = NewValue;

	EquipmentComponent->SwapMainAndSecondaryEquipmentSlots();
}

void AInGameCharacter::SetIsAlive(bool NewValue)
{
	bIsAlive = NewValue;
}

bool AInGameCharacter::IsAlive()
{
	return bIsAlive;
}

void AInGameCharacter::ShowPlayerInfo()
{
	FString Prefix;
	if (HasAuthority())
	{
		Prefix = "Server:";
	}
	else if (IsLocallyControlled())
	{
		Prefix = "Owning Client:";
	}
	else
	{
		Prefix = "Client:";
	}

	UE_LOG(LogTemp, Warning, TEXT("%s ===== Character stats ====="), *Prefix);

	if (GetFocusedInteractableObject() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject is a valid pointer"), *Prefix);

		IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(GetFocusedInteractableObject());
		if (InteractableActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject implements IInteractableInterface"), *Prefix);
			if (InteractableActor->bCanInteract(GetController()))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject can be interacted with"), *Prefix);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject cannot be interacted with"), *Prefix);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject == nullptr"), *Prefix);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Player has %d equipment slots"), *Prefix, EquipmentComponent->GetEquipmentSlots().Num());
	UE_LOG(LogTemp, Warning, TEXT("%s Player has %d equipment sockets"), *Prefix, EquipmentComponent->GetEquipmentSockets().Num());

	int i = 0;
	for (TPair<FGameplayTag, AItemBase*> Slot : EquipmentComponent->GetEquipmentSlots())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Slot #%d - %s"), *Prefix, i, *(Slot.Key.ToString()));
		if (IsValid(Slot.Value))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s has a valid item equiped"), *Prefix, *(Slot.Key.ToString()));
			if (Slot.Value->bCanInteract(GetController()))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s %s can be interacted with, THIS SHOULD NOT BE POSSIBLE"), *Prefix, *(Slot.Key.ToString()));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s %s cannot be interacted with as it is equiped by the player"), *Prefix, *(Slot.Key.ToString()));
			}
			UE_LOG(LogTemp, Warning, TEXT("%s %s is a %s"), *Prefix, *(Slot.Key.ToString()), *(Slot.Value->ItemData.ItemName.ToString()));
			UE_LOG(LogTemp, Warning, TEXT("%s %s Durability is %f out of %f - %f percent"), *Prefix, *(Slot.Key.ToString()), Slot.Value->GetDurability(), 
				Slot.Value->ItemData.ItemMaxDurability, Slot.Value->GetDurability() / Slot.Value->ItemData.ItemMaxDurability * 100);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s is nullptr"), *Prefix, *(Slot.Key.ToString()));
		}
		if (EquipmentComponent->HasEquipmentSocketWithTag(Slot.Key))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s has equipment socket with name %s"), *Prefix, *(Slot.Key.ToString()), *(EquipmentComponent->GetEquipmentSocketWithTag(Slot.Key).ToString()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s does not have an associated equipment socket!"), *Prefix, *(Slot.Key.ToString()));
		}

		i++;
	}
}


