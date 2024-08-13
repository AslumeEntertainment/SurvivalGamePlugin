// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SG_GameplayAbility.h"
#include "SGGA_CraftItem.generated.h"


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
	class USG_AnimInstance* AnimInst;

	class AItemBase* ProductItem;
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
