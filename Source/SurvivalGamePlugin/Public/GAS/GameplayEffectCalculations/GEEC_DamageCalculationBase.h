// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"

#include "GameplayTagContainer.h"

#include "GEEC_DamageCalculationBase.generated.h"

class USG_AbilitySystemComponent;
class UDamageSourceData;

UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_DamageCalculationBase : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	bool bShouldPrintDebug = true;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	bool bShouldTriggerCuesOnZeroMagnitude = true;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues", meta = (Categories = "GameplayCue"))
	FGameplayTagContainer GameplayCuesToExecuteOnSuccess;


	float GetStrengthMultiplier(USG_AbilitySystemComponent* InstigatorASC, AActor* EffectCauser) const;

	float GetPenetrationMultiplier(const UDamageSourceData* DamageSourceData, float InstigatorVelocity, USG_AbilitySystemComponent* TargetASC, FVector HitLocation) const;

	void TriggerGameplayCues(USG_AbilitySystemComponent* TargetASC, FGameplayCueParameters CueParams) const;
};
