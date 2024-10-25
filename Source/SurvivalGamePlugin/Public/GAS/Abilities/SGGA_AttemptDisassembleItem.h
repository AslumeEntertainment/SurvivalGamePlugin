// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "Headers/ItemDataStruct.h"
#include "SGGA_AttemptDisassembleItem.generated.h"

class AInGameCharacter;
class AItemBase;
class UEquipmentComponent;
class UGameplayEffect;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_AttemptDisassembleItem : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "DisassemblyInfo")
	UDataTable* DisassemlyInfoTable;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> SpeedReductionEffect;

	AInGameCharacter* Player;
	UEquipmentComponent* EquipmentComp;

	FActiveGameplayEffectHandle SpeedReductionEffectSpecHandle;

	FDisassemblyProcessDataStruct* ItemForDisassemblyData = nullptr;

	UPROPERTY()
	AItemBase* MainItemActor;
	UPROPERTY()
	AItemBase* IngredientItemActor;

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageCanceled();

	UFUNCTION()
	void OnDisassemblyFinished(FGameplayEventData Payload);
};
