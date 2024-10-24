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

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag MainEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag SecondaryEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag MixedEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag CombinedEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag CombinedMainEquipmentSlotTag;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterDefaultTags")
	FGameplayTag CombinedSecondaryEquipmentSlotTag;

	void PackageEquipment(FPackagedEquipmentData& OutEquipment);

	void ReconstructEquipmentData(const FPackagedEquipmentData& EquipmentData);

	UFUNCTION()
	void OnRep_CachedEquipment();


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<FGameplayTag, AItemBase*> EquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<FGameplayTag, FName> EquipmentSockets;


public:

	FGameplayTag GetMainEquipmentSlotTag()                { return MainEquipmentSlotTag; }
	FGameplayTag GetSecondaryEquipmentSlotTag()           { return SecondaryEquipmentSlotTag; }
	FGameplayTag GetMixedEquipmentSlotTag()               { return MixedEquipmentSlotTag; }
	FGameplayTag GetCombinedEquipmentSlotTag()            { return CombinedEquipmentSlotTag; }
	FGameplayTag GetCombinedMainEquipmentSlotTag()        { return CombinedMainEquipmentSlotTag; }
	FGameplayTag GetCombinedSecondaryEquipmentSlotTag()   { return CombinedSecondaryEquipmentSlotTag; }

	void SwapMainAndSecondaryEquipmentSlots();

	UFUNCTION(BlueprintCallable)
	bool HasEquipmentSlotWithTag(FGameplayTag SlotTag, bool CheckParentTag = true) const;

	UFUNCTION(BlueprintCallable)
	void SetEquipedItemAtSlot(FGameplayTag SlotTag, AItemBase* Item, bool CheckParentTag = true);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetEquipedItemAtSlot(FGameplayTag SlotTag, AItemBase* Item, bool CheckParentTag = true);

	UFUNCTION(BlueprintCallable)
	AItemBase* GetEquipedItemAtSlot(FGameplayTag SlotTag, bool CheckParentTag = true) const;

	UFUNCTION(BlueprintCallable)
	TArray<AItemBase*> GetAllEquipedItems() const;

	UFUNCTION(BlueprintCallable)
	void UnequipAllItems();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UnequipAllItems();

	UFUNCTION(BlueprintCallable)
	TArray<ADamageDealingItem*> GetAllEquipedWeapons() const;

	UFUNCTION(BlueprintCallable)
	bool HasEquipmentSocketWithTag(FGameplayTag SlotTag, bool CheckParentTag = true) const;

	UFUNCTION(BlueprintCallable)
	FName GetEquipmentSocketWithTag(FGameplayTag SlotTag, bool CheckParentTag = true) const;

	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, AItemBase*> GetEquipmentSlots() const;

	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, FName> GetEquipmentSockets() const;
};
