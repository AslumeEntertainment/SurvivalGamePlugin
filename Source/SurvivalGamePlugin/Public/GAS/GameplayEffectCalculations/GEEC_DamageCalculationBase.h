// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"

#include "Objects/GameplayEffectDataObjects/DamageSourceData.h"
#include "GameplayTagContainer.h"

#include "GEEC_DamageCalculationBase.generated.h"

class USG_AbilitySystemComponent;
class UDamageSourceData;

DECLARE_LOG_CATEGORY_EXTERN(DamageLog, Log, All);

UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_DamageCalculationBase : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	bool bShouldPrintDebug = true;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	bool bShouldTriggerCuesOnZeroMagnitude = true;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues", meta = (Categories = "Bool"))
	TMap<FGameplayTag, ESG_DamageType> DamageImmunityForTagsOwned;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues", meta = (Categories = "GameplayCue"))
	FGameplayTagContainer GameplayCuesToExecuteOnSuccess;

	bool IsTargetImmuneToDamageType(USG_AbilitySystemComponent* TargetASC, ESG_DamageType DamageType) const;

	void TriggerGameplayCues(USG_AbilitySystemComponent* TargetASC, FGameplayCueParameters CueParams) const;

	void ApplyAdditionalEffects(USG_AbilitySystemComponent* TargetASC, TArray<FAdditionalEffectDataStruct> AdditionalEffectsData, FGameplayEffectContextHandle EffectContext, float InstigatorDexterity = 0.f) const;

	void AcknowledgeHit(USG_AbilitySystemComponent* InstigatorASC, USG_AbilitySystemComponent* TargetASC, FGameplayEventData EventData) const;
};
