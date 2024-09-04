// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "Headers/ItemDataStruct.h"
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

	AHumanCharacter* Player;
	USG_AnimInstance* AnimInst;

	FDisassemblyProcessDataStruct* ItemForDisassemblyData = nullptr;
	AItemBase** ItemForDisassembly = nullptr;
	AItemBase** OtherItem = nullptr;

	UPROPERTY(Replicated)
	AItemBase* MainItemActor;
	UPROPERTY(Replicated)
	AItemBase* IngredientItemActor;

	UPROPERTY(Replicated)
	FName MainItemSocket;
	UPROPERTY(Replicated)
	FName IngredientItemSocket;

	bool bCanDisassembleItem(AItemBase* MainItemForDisassembly, AItemBase* OtherEquipedItem);

	UFUNCTION()
	void SpawnNewItems();

	UFUNCTION()
	void OnAnimMontageCompleted(/*FGameplayTag EventTag, FGameplayEventData Payload*/);

	UFUNCTION()
	void OnAnimMontageCanceled(/*FGameplayTag EventTag, FGameplayEventData Payload*/);

	UFUNCTION()
	void OnDisassemblyFinished(/*FGameplayTag EventTag, */FGameplayEventData Payload);
};
