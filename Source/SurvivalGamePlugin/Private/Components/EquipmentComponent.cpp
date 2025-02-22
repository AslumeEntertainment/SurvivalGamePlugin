// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"

#include "Net/UnrealNetwork.h"

#include "Interactables/Items/DamageDealingItem.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"

bool FPackagedEquipmentData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	SafeNetSerializeTArray_WithNetSerialize<5>(Ar, SlotTags, Map);
	SafeNetSerializeTArray_Default<5>(Ar, EquipedItems);

	bOutSuccess = true;
	return true;
}

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MainEquipmentSlotTag                 = FGameplayTag::RequestGameplayTag("EquipmentSlot.Main");
	SecondaryEquipmentSlotTag            = FGameplayTag::RequestGameplayTag("EquipmentSlot.Secondary");
	MixedEquipmentSlotTag                = FGameplayTag::RequestGameplayTag("EquipmentSlot.Mixed");
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UEquipmentComponent, CachedEquipment, COND_None, REPNOTIFY_Always);
}

void UEquipmentComponent::PackageEquipment(FPackagedEquipmentData& OutEquipment)
{
	OutEquipment.SlotTags.Empty();
	OutEquipment.EquipedItems.Empty();

	for (const auto& Pair : EquipmentSlots)
	{
		OutEquipment.SlotTags.Add(Pair.Key);
		OutEquipment.EquipedItems.Add(Pair.Value);
	}
}

void UEquipmentComponent::ReconstructEquipmentData(const FPackagedEquipmentData& EquipmentData)
{
	EquipmentSlots.Empty();

	for (uint8 i = 0; i < EquipmentData.SlotTags.Num(); i++)
	{
		EquipmentSlots.Emplace(EquipmentData.SlotTags[i], EquipmentData.EquipedItems[i]);
	}
}

void UEquipmentComponent::OnRep_CachedEquipment()
{
	ReconstructEquipmentData(CachedEquipment);
}

bool UEquipmentComponent::CanEquipItem(AItemBase* Item)
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::CanEquipItem IsValid(GetOwner()) == false"));
		return false;
	}
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::CanEquipItem IsValid(ASC) == false"));
		return false;
	}

	float OwnerStrength = ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetStrengthAttribute());
	EWeight LastTorelableWeight;

	for (EWeight Weight : TEnumRange<EWeight>())
	{
		if (UEnum::GetValueAsString(Weight).Compare("None") == 0) continue;

		if (OwnerStrength < (uint8)Weight)
		{
			LastTorelableWeight = Weight;
			break;
		}
	}
	
	if (Item->ItemData.ItemWeight > LastTorelableWeight)
	{
		return false;
	}

	return true;
}

void UEquipmentComponent::SetEquipedItemAtSlot(FGameplayTag SlotTag, AItemBase* Item)
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::SetEquipedItemAtSlot IsValid(GetOwner()) == false"));
		return;
	}
	if (GetOwner()->HasAuthority() == false)
	{
		Server_SetEquipedItemAtSlot(SlotTag, Item);
		return;
	}
	if (HasEquipmentSlotWithTag(SlotTag) == false)
	{
		return;
	}

	EquipmentSlots[SlotTag] = Item;
	PackageEquipment(CachedEquipment);
}

void UEquipmentComponent::Server_SetEquipedItemAtSlot_Implementation(FGameplayTag SlotTag, AItemBase* Item)
{
	SetEquipedItemAtSlot(SlotTag, Item);
}

void UEquipmentComponent::EquipItem(AItemBase* Item, bool bShouldSlotBeEmpty)
{
	AInGameCharacter* InGameCharacter = Cast<AInGameCharacter>(GetOwner());
	if (IsValid(InGameCharacter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem IsValid(InGameCharacter) == false"));
		return;
	}
	if (InGameCharacter->HasAuthority() == false)
	{
		Server_EquipItem(Item, bShouldSlotBeEmpty);
		return;
	}
	if (IsValid(Item) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem IsValid(Item) == false"));
		return;
	}
	UAbilitySystemComponent* ASC = InGameCharacter->GetAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem IsValid(ASC) == false"));
		return;
	}

	float OwnerStrength = ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetStrengthAttribute());
	FGameplayTag EquipmentSlotTag = Item->ItemData.EquipmentSlotTag;
	EWeight ItemWeight = Item->ItemData.ItemWeight;
	TArray<EWeight> EquipedItemsWeights;
	
	for (const TPair<FGameplayTag, AItemBase*> EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value))
		{
			EquipedItemsWeights.Add(EquipmentSlot.Value->ItemData.ItemWeight);
		}
	}

	if (HasEquipmentSlotWithTag(MixedEquipmentSlotTag))
	{
		EquipmentSlotTag = MixedEquipmentSlotTag;
		if ((uint8)ItemWeight > OwnerStrength)
		{
			return;
		}
	}
	if (bShouldSlotBeEmpty && IsValid(GetEquipedItemAtSlot(EquipmentSlotTag)))
	{
		return;
	}

	if ((uint8)ItemWeight > OwnerStrength)
	{
		UnequipAllItems();
	}
	for (EWeight EquipedItemWeight : EquipedItemsWeights)
	{
		if ((uint8)EquipedItemWeight > OwnerStrength) UnequipAllItems(); break;
	}

	if (IsValid(GetEquipedItemAtSlot(EquipmentSlotTag)))
	{
		GetEquipedItemAtSlot(EquipmentSlotTag)->OnItemUnequiped();
		SetEquipedItemAtSlot(EquipmentSlotTag, nullptr);
	}

	SetEquipedItemAtSlot(EquipmentSlotTag, Item);
	Item->OnItemEquiped(InGameCharacter, GetEquipmentSocketWithTag(EquipmentSlotTag));

	return;
}

void UEquipmentComponent::Server_EquipItem_Implementation(AItemBase* Item, bool bShouldSlotBeEmpty)
{
	EquipItem(Item, bShouldSlotBeEmpty);
}

void UEquipmentComponent::UnequipItem(AItemBase* Item)
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::UnequipItem IsValid(GetOwner()) == false"));
		return;
	}
	if (GetOwner()->HasAuthority() == false)
	{
		Server_UnequipItem(Item);
		return;
	}

	if (IsValid(Item) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::UnequipItem IsValid(Item) == false"));
		return;
	}

	for (TPair<FGameplayTag, AItemBase*> EquipmentSlot : EquipmentSlots)
	{
		if (EquipmentSlot.Value == Item)
		{
			Item->OnItemUnequiped();
			SetEquipedItemAtSlot(EquipmentSlot.Key, nullptr);
		}
	}

	return;
}

void UEquipmentComponent::UnequipItemAtSlot(FGameplayTag SlotTag)
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::UnequipItemAtSlot IsValid(GetOwner()) == false"));
		return;
	}
	if (GetOwner()->HasAuthority() == false)
	{
		Server_UnequipItemAtSlot(SlotTag);
		return;
	}

	if (IsValid(GetEquipedItemAtSlot(SlotTag)))
	{
		GetEquipedItemAtSlot(SlotTag)->OnItemUnequiped();
		SetEquipedItemAtSlot(SlotTag, nullptr);
	}

	return;
}

void UEquipmentComponent::Server_UnequipItemAtSlot_Implementation(FGameplayTag SlotTag)
{
	UnequipItemAtSlot(SlotTag);
}

void UEquipmentComponent::Server_UnequipItem_Implementation(AItemBase* Item)
{
	UnequipItem(Item);
}

int UEquipmentComponent::GetNumberOfEquipedItems() const
{
	int i = 0;

	for (TPair<FGameplayTag, AItemBase*> EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value))
		{
			i++;
		}
	}

	return i;
}


bool UEquipmentComponent::HasEquipmentSlotWithTag(FGameplayTag SlotTag) const
{
	return EquipmentSlots.Contains(SlotTag);
}



AItemBase* UEquipmentComponent::GetEquipedItemAtSlot(FGameplayTag SlotTag) const
{
	if (HasEquipmentSlotWithTag(SlotTag) == false)
	{
		return nullptr;
	}

	return EquipmentSlots[SlotTag];
}

TArray<AItemBase*> UEquipmentComponent::GetAllEquipedItems(bool bIsReversed, bool bIncludeNullItems) const
{
	TArray<AItemBase*> EquipedItems;
	for (TPair<FGameplayTag, AItemBase*> EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value) == false && bIncludeNullItems == false)
		{
			continue;
		}

		if (bIsReversed)
		{
			EquipedItems.Insert(EquipmentSlot.Value, 0);
		}
		else
		{
			EquipedItems.Add(EquipmentSlot.Value);
		}
		
	}
	return EquipedItems;
}

void UEquipmentComponent::UnequipAllItems()
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::UnequipAllItems IsValid(GetOwner()) == false"));
		return;
	}
	if (GetOwner()->HasAuthority() == false)
	{
		Server_UnequipAllItems();
		return;
	}
	for (TPair<FGameplayTag, AItemBase*> EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value))
		{
			UnequipItem(EquipmentSlot.Value);
		}
	}

	PackageEquipment(CachedEquipment);
}

void UEquipmentComponent::Server_UnequipAllItems_Implementation()
{
	UnequipAllItems();
}

TArray<ADamageDealingItem*> UEquipmentComponent::GetAllEquipedWeapons() const
{
	TArray<ADamageDealingItem*> EquipedWeapons;
	for (TPair<FGameplayTag, AItemBase*> EquipmentSlot : EquipmentSlots)
	{
		ADamageDealingItem* EquipedWeapon = Cast<ADamageDealingItem>(EquipmentSlot.Value);
		if (IsValid(EquipedWeapon))
		{
			EquipedWeapons.Add(EquipedWeapon);
		}
	}
	return EquipedWeapons;
}

bool UEquipmentComponent::HasEquipmentSocketWithTag(FGameplayTag SlotTag) const
{
	return EquipmentSockets.Contains(SlotTag);
}

FName UEquipmentComponent::GetEquipmentSocketWithTag(FGameplayTag SlotTag) const
{
	if (HasEquipmentSocketWithTag(SlotTag) == false)
	{
		return FName();
	}
	
	return EquipmentSockets[SlotTag];
}

TMap<FGameplayTag, AItemBase*> UEquipmentComponent::GetEquipmentSlots() const
{
	return EquipmentSlots;
}

TMap<FGameplayTag, FName> UEquipmentComponent::GetEquipmentSockets() const
{
	return EquipmentSockets;
}
