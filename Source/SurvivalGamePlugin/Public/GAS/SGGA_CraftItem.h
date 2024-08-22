// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SG_GameplayAbility.h"

#include "Headers/ItemDataStruct.h"

#include "SGGA_CraftItem.generated.h"

class AHumanCharacter;
class USG_AnimInstance;
//class AItemBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_CraftItem : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "CraftingInfo")
	UDataTable* CraftingInfoTable;

	AHumanCharacter* Player;
	USG_AnimInstance* AnimInst;

	AItemBase* ProductItem;
	FName ProductItemSocketName;

	FCraftingProcessDataStruct* ProductItemData = nullptr;

	UFUNCTION()
	void SpawnProductItem();

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageCanceled();

	UFUNCTION()
	void OnCraftingFinished(FGameplayEventData Payload);

};
