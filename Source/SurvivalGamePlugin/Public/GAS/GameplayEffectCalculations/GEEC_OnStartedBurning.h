// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_OnStartedBurning.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_OnStartedBurning : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	TSubclassOf<UGameplayEffect> BurningEffect;

	/** This will be replaced when we set up the fire related gameplay attributes */
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues", meta = (ClampMin = "1", ClampMax = "100", UIMin = "1", UIMax = "100"))
	int ChanceToSpread = 50;
};
