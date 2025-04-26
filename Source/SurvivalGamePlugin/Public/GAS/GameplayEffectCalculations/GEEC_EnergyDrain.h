// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_EnergyDrain.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_EnergyDrain : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UGEEC_EnergyDrain();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
