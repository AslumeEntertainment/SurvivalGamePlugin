// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/InGameCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

#include "Interactables/Items/DamageDealingItem.h"
#include "Components/DamageDealingComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"


AInGameCharacter::AInGameCharacter()
{
	LeftHandDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("LeftHandDamageOrigin"));
	LeftHandDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("hand_l"));

	RightHandDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("RightHandDamageOrigin"));
	RightHandDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("hand_r"));
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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHungerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerHunger(Data.NewValue, AttributeSet->GetMaxHunger());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerEnergy(Data.NewValue, AttributeSet->GetMaxEnergy());
		}
	);
}

void AInGameCharacter::BroadcastInitialValues()
{
	// empty for now
}

void AInGameCharacter::OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles)
{
	for (FGameplayEffectSpecHandle SpecHandle : GameplayEffectSpecHandles)
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

bool AInGameCharacter::bCanBeHit()
{
	return IsAlive();
}

void AInGameCharacter::OnHittableDestroyed_Implementation()
{
	bIsAlive = false;

	OnCharacterDeath.Broadcast();

	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	GetCapsuleComponent()->SetCollisionProfileName(FName("NoCollision"));

	UnPossessed();
}

void AInGameCharacter::Client_OnUpdatePlayerHealth_Implementation(float CurrentHealth, float MaxHealth)
{
	OnHealthPercentUpdated.Broadcast(CurrentHealth / MaxHealth);
}

void AInGameCharacter::Client_OnUpdatePlayerHunger_Implementation(float CurrentHunger, float MaxHunger)
{
	if (CurrentHunger <= MaxHunger / 2)
	{
		OnHungerPercentUpdated.Broadcast(CurrentHunger / MaxHunger);
	}
}

void AInGameCharacter::Client_OnUpdatePlayerEnergy_Implementation(float CurrentEnergy, float MaxEnergy)
{
	if (CurrentEnergy <= MaxEnergy / 2)
	{
		OnEnergyPercentUpdated.Broadcast(CurrentEnergy / (MaxEnergy / 2));
	}
}

bool AInGameCharacter::HasEquipmentSlotOfType(EEquipmentSlot SlotType) const
{
	return EquipmentSlots.Contains(SlotType);
}

void AInGameCharacter::SetEquipedItemAtSlot_Implementation(EEquipmentSlot SlotType, AItemBase* Item)
{
	if (HasEquipmentSlotOfType(SlotType) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameCharacter: Player cannot equip item because they don't have the slot for it!"));
		return;
	}

	EquipmentSlots[SlotType] = Item;
	return;
}

AItemBase* AInGameCharacter::GetEquipedItemAtSlot(EEquipmentSlot SlotType) const
{
	if (HasEquipmentSlotOfType(SlotType) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameCharacter: Player does not have an equipment slot of such type!"));
		return nullptr;
	}

	return EquipmentSlots[SlotType];
}

TArray<ADamageDealingItem*> AInGameCharacter::GetAllEquipedWeapons() const
{
	TArray<ADamageDealingItem*> EquipedWeapons;
	for (auto& EquipmentSlot : EquipmentSlots)
	{
		ADamageDealingItem* EquipedWeapon = Cast<ADamageDealingItem>(EquipmentSlot.Value);
		if (IsValid(EquipedWeapon))
		{
			EquipedWeapons.Add(EquipedWeapon);
		}
	}
	return EquipedWeapons;
}

FName AInGameCharacter::GetEquipmentSocketOfType(EEquipmentSlot SlotType) const
{
	if (HasEquipmentSlotOfType(SlotType) == false)
	{
		return FName();
	}

	return EquipmentSockets[SlotType];
}

bool AInGameCharacter::IsAlive()
{
	return bIsAlive;
}
