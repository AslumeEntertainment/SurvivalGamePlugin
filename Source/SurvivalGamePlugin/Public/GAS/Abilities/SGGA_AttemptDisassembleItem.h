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
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "CraftingInfo")
	UDataTable* DisassemlyInfoTable;

	AHumanCharacter* Player;
	USG_AnimInstance* AnimInst;
	float OldSpeed;

	FDisassemblyProcessDataStruct* ItemForDisassemblyData = nullptr;

	UPROPERTY(Replicated)
	AItemBase* MainItemActor;
	UPROPERTY(Replicated)
	AItemBase* IngredientItemActor;

	UFUNCTION()
	void SpawnNewItems();

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageCanceled();

	UFUNCTION()
	void OnDisassemblyFinished(FGameplayEventData Payload);
};
