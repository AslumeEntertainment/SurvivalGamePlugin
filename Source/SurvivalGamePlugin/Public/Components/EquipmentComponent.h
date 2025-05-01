// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameplayTagContainer.h"

#include "EquipmentComponent.generated.h"

class AItemBase;
class ADamageDealingItem;

USTRUCT()
struct FPackagedEquipmentData
{
	GENERATED_BODY()

	virtual ~FPackagedEquipmentData() = default;

	UPROPERTY()
	TArray<FGameplayTag> SlotTags;

	UPROPERTY()
	TArray<AItemBase*> EquipedItems;

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

};
template<>
struct TStructOpsTypeTraits<FPackagedEquipmentData> : TStructOpsTypeTraitsBase2<FPackagedEquipmentData>
{
	enum
	{
		WithNetSerializer = true
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALGAMEPLUGIN_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:

	UPROPERTY(ReplicatedUsing = OnRep_CachedEquipment)
	FPackagedEquipmentData CachedEquipment;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags", meta = (Categories = "EquipmentSlot"))
	FGameplayTag MainEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags", meta = (Categories = "EquipmentSlot"))
	FGameplayTag SecondaryEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags", meta = (Categories = "EquipmentSlot"))
	FGameplayTag MixedEquipmentSlotTag;

	void PackageEquipment(FPackagedEquipmentData& OutEquipment);

	void ReconstructEquipmentData(const FPackagedEquipmentData& EquipmentData);

	UFUNCTION()
	void OnRep_CachedEquipment();


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (Categories = "EquipmentSlot"))
	TMap<FGameplayTag, AItemBase*> EquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (Categories = "EquipmentSlot"))
	TMap<FGameplayTag, FName> EquipmentSockets;

	//tova beshe protected ama mi trqbva za da moga da karam itemi da izchezvat akto gi mestq v deck-a
	UFUNCTION(BlueprintCallable)
	void SetEquipedItemAtSlot(FGameplayTag SlotTag, AItemBase* Item);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetEquipedItemAtSlot(FGameplayTag SlotTag, AItemBase* Item);

public:

	FGameplayTag GetMainEquipmentSlotTag()                { return MainEquipmentSlotTag; }
	FGameplayTag GetSecondaryEquipmentSlotTag()           { return SecondaryEquipmentSlotTag; }
	FGameplayTag GetMixedEquipmentSlotTag()               { return MixedEquipmentSlotTag; }

	UFUNCTION()
	void VanishItemAtSlot(FGameplayTag SlotTag);

	UFUNCTION(Server, Reliable)
	void Server_VanishItemAtSlot(FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable)
	bool CanEquipItem(const AItemBase* Item);

	UFUNCTION(BlueprintCallable)
	void EquipItem(AItemBase* Item, bool bShouldSlotBeEmpty = false);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_EquipItem(AItemBase* Item, bool bShouldSlotBeEmpty = false);

	UFUNCTION(BlueprintCallable)
	void UnequipItem(AItemBase* Item);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UnequipItem(AItemBase* Item);

	UFUNCTION(BlueprintCallable)
	void UnequipItemAtSlot(FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UnequipItemAtSlot(FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable)
	void UnequipAllItems();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UnequipAllItems();

	UFUNCTION(BlueprintCallable)
	bool HasEquipmentSlotWithTag(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable)
	int GetNumberOfEquipedItems() const;

	UFUNCTION(BlueprintCallable)
	AItemBase* GetEquipedItemAtSlot(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable)
	TArray<AItemBase*> GetAllEquipedItems(bool bIsReversed = false, bool bIncludeNullItems = false) const;

	UFUNCTION(BlueprintCallable)
	TArray<ADamageDealingItem*> GetAllEquipedWeapons() const;

	UFUNCTION(BlueprintCallable)
	bool HasEquipmentSocketWithTag(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable)
	FName GetEquipmentSocketWithTag(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, AItemBase*> GetEquipmentSlots() const;

	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, FName> GetEquipmentSockets() const;

};
