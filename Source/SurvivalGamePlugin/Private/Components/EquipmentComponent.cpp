// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"

#include "Net/UnrealNetwork.h"

#include "Interactables/Items/DamageDealingItem.h"
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
	
void UEquipmentComponent::SwapMainAndSecondaryEquipmentSlots()
{
	if (EquipmentSlots.Contains(GetMainEquipmentSlotTag()) && EquipmentSlots.Contains(GetSecondaryEquipmentSlotTag()))
	{
		FName Temp = EquipmentSockets[GetMainEquipmentSlotTag()];
		EquipmentSockets[GetMainEquipmentSlotTag()] = EquipmentSockets[GetSecondaryEquipmentSlotTag()];
		EquipmentSockets[GetSecondaryEquipmentSlotTag()] = Temp;
	}
}

bool UEquipmentComponent::EquipItem(AItemBase* Item, bool bShouldSlotBeEmpty)
{
	if (IsValid(Item) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem IsValid(Item) == false"));
		return false;
	}

	UAbilitySystemComponent* ASC = USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem IsValid(ASC) == false"));
		return false;
	}

	AInGameCharacter* InGameCharacter = Cast<AInGameCharacter>(GetOwner());
	if(IsValid(InGameCharacter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem IsValid(InGameCharacter) == false"));
		return false;
	}

	FGameplayTag EquipmentSlotTag = Item->ItemData.EquipmentSlotTag;

	EWeight ItemWeight = Item->ItemData.ItemWeight;
	TArray<EWeight> EquipedItemsWeights;

	for (TPair<FGameplayTag, AItemBase*> EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value))
		{
			EquipedItemsWeights.Add(EquipmentSlot.Value->ItemData.ItemWeight);
		}
	}

	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasBarbarianStrength")))
	{
		ItemWeight = (EWeight)FMath::Clamp((uint8)ItemWeight - 1, 0, 3);
		for (int i = 0; i < EquipedItemsWeights.Num(); i++)
		{
			EquipedItemsWeights[i] = (EWeight)FMath::Clamp((uint8)EquipedItemsWeights[i] - 1, 0, 3);
		}
	}

	if (HasEquipmentSlotWithTag(MixedEquipmentSlotTag))
	{
		EquipmentSlotTag = MixedEquipmentSlotTag;
		if (ItemWeight == EWeight::Heavy)
		{
			return false;
		}
	}
	if (bShouldSlotBeEmpty && IsValid(GetEquipedItemAtSlot(EquipmentSlotTag)))
	{
		return false;
	}

	if (ItemWeight == EWeight::Heavy || EquipedItemsWeights.Contains(EWeight::Heavy))
	{
		UnequipAllItems();
	}

	if (IsValid(GetEquipedItemAtSlot(EquipmentSlotTag)))
	{
		GetEquipedItemAtSlot(EquipmentSlotTag)->OnItemUnequiped();
		SetEquipedItemAtSlot(EquipmentSlotTag, nullptr);
	}

	SetEquipedItemAtSlot(EquipmentSlotTag, Item);
	Item->OnItemEquiped(InGameCharacter, GetEquipmentSocketWithTag(EquipmentSlotTag));

	return true;
}

bool UEquipmentComponent::UnequipItem(AItemBase* Item)
{
	if (IsValid(Item) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::UnequipItem IsValid(Item) == false"));
		return false;
	}

	for (TPair<FGameplayTag, AItemBase*> EquipmentSlot : EquipmentSlots)
	{
		if (EquipmentSlot.Value == Item)
		{
			Item->OnItemUnequiped();
			SetEquipedItemAtSlot(EquipmentSlot.Key, nullptr);
		}
	}

	return true;
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
