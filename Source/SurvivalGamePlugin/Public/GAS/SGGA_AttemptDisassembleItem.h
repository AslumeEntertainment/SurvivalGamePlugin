// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SG_GameplayAbility.h"
#include "SGGA_AttemptDisassembleItem.generated.h"

class AHumanCharacter;
class AItemBase;
class USG_AnimInstance;
class UGameplayEffect;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_AttemptDisassembleItem : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "CraftingInfo")
	UDataTable* DisassemlyInfoTable;

	UPROPERTY(EditDefaultsOnly, Category = "CraftingInfo")
	TSubclassOf<UGameplayEffect> SpeedReductionEffect;

	AHumanCharacter* Player;
	USG_AnimInstance* AnimInst;

	FDisassemblyProcessDataStruct* ItemForDisassemblyData = nullptr;
	AItemBase** ItemForDisassembly = nullptr;
	AItemBase** OtherItem = nullptr;

	FName MainItemSocket;
	FName IngredientItemSocket;

	AItemBase* MainItemActor;
	AItemBase* IngredientItemActor;

	bool bCanDisassembleItem(AItemBase* MainItemForDisassembly, AItemBase* OtherEquipedItem);

	UFUNCTION()
	void SpawnNewItems();

	UFUNCTION()
	void OnAnimMontageBlendOut();

	UFUNCTION()
	void OnAnimMontageCanceled();

	UFUNCTION()
	void OnDisassemblyFinished(FGameplayEventData Payload);
};
