// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"

#include "Net/UnrealNetwork.h"

#include "Interactables/Items/DamageDealingItem.h"

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
	CombinedEquipmentSlotTag             = FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined");
	CombinedMainEquipmentSlotTag         = FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined.Main");
	CombinedSecondaryEquipmentSlotTag    = FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined.Secondary");
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
	
void UEquipmentComponent::SwapMainAndSecondaryEquipmentSlots()
{
	if (EquipmentSlots.Contains(GetMainEquipmentSlotTag()) && EquipmentSlots.Contains(GetSecondaryEquipmentSlotTag()))
	{
		FName Temp = EquipmentSockets[GetMainEquipmentSlotTag()];
		EquipmentSockets[GetMainEquipmentSlotTag()] = EquipmentSockets[GetSecondaryEquipmentSlotTag()];
		EquipmentSockets[GetSecondaryEquipmentSlotTag()] = Temp;
	}
}

bool UEquipmentComponent::HasEquipmentSlotWithTag(FGameplayTag SlotTag, bool CheckParentTag) const
{
	if (CheckParentTag)
	{
		return EquipmentSlots.Contains(SlotTag) || EquipmentSlots.Contains(SlotTag.RequestDirectParent());
	}

	return EquipmentSlots.Contains(SlotTag);
}

void UEquipmentComponent::SetEquipedItemAtSlot(FGameplayTag SlotTag, AItemBase* Item, bool CheckParentTag)
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::SetEquipedItemAtSlot IsValid(GetOwner()) == false"));
		return;
	}
	if (GetOwner()->HasAuthority() == false)
	{
		Server_SetEquipedItemAtSlot(SlotTag, Item, CheckParentTag);
		return;
	}

	if (HasEquipmentSlotWithTag(SlotTag, CheckParentTag) == false)
	{
		return;
	}
	if (HasEquipmentSlotWithTag(SlotTag, false) == false)
	{
		EquipmentSlots[SlotTag.RequestDirectParent()] = Item;
	}
	else
	{
		EquipmentSlots[SlotTag] = Item;
	}

	PackageEquipment(CachedEquipment);
}

void UEquipmentComponent::Server_SetEquipedItemAtSlot_Implementation(FGameplayTag SlotTag, AItemBase* Item, bool CheckParentTag)
{
	SetEquipedItemAtSlot(SlotTag, Item, CheckParentTag);
}

AItemBase* UEquipmentComponent::GetEquipedItemAtSlot(FGameplayTag SlotTag, bool CheckParentTag) const
{
	if (HasEquipmentSlotWithTag(SlotTag, CheckParentTag) == false)
	{
		return nullptr;
	}

	if (HasEquipmentSlotWithTag(SlotTag, false) == false)
	{
		return EquipmentSlots[SlotTag.RequestDirectParent()];
	}
	else
	{
		return EquipmentSlots[SlotTag];
	}
}

TArray<AItemBase*> UEquipmentComponent::GetAllEquipedItems() const
{
	TArray<AItemBase*> EquipedItems;
	for (auto& EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value))
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
	for (auto& EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value))
		{
			EquipmentSlot.Value->OnItemUnequiped();
			EquipmentSlot.Value = nullptr;
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

bool UEquipmentComponent::HasEquipmentSocketWithTag(FGameplayTag SlotTag, bool CheckParentTag) const
{
	if (CheckParentTag)
	{
		return EquipmentSockets.Contains(SlotTag) || EquipmentSockets.Contains(SlotTag.RequestDirectParent());
	}

	return EquipmentSockets.Contains(SlotTag);
}

FName UEquipmentComponent::GetEquipmentSocketWithTag(FGameplayTag SlotTag, bool CheckParentTag) const
{
	if (HasEquipmentSocketWithTag(SlotTag, CheckParentTag) == false)
	{
		return FName();
	}
	if (HasEquipmentSocketWithTag(SlotTag, false) == false)
	{
		return EquipmentSockets[SlotTag.RequestDirectParent()];
	}
	else
	{
		return EquipmentSockets[SlotTag];
	}
}

TMap<FGameplayTag, AItemBase*> UEquipmentComponent::GetEquipmentSlots() const
{
	return EquipmentSlots;
}

TMap<FGameplayTag, FName> UEquipmentComponent::GetEquipmentSockets() const
{
	return EquipmentSockets;
}
