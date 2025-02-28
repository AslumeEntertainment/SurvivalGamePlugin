// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_CalculateDamage.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_CalculateDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UGEEC_CalculateDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
