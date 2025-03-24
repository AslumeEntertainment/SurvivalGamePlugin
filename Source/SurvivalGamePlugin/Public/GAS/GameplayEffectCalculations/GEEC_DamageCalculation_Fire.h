// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculationBase.h"
#include "GEEC_DamageCalculation_Fire.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_DamageCalculation_Fire : public UGEEC_DamageCalculationBase
{
	GENERATED_BODY()
	
public:

	UGEEC_DamageCalculation_Fire();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
