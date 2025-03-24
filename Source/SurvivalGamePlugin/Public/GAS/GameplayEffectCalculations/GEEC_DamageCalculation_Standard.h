// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculationBase.h"
#include "GEEC_DamageCalculation_Standard.generated.h"

class AActor;
class UDamageSourceData;
class USG_AbilitySystemComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_DamageCalculation_Standard : public UGEEC_DamageCalculationBase
{
	GENERATED_BODY()
	
public:

	UGEEC_DamageCalculation_Standard();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	

};
