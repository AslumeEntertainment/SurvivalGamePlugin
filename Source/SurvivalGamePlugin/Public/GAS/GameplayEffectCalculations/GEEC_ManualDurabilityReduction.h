// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_ManualDurabilityReduction.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_ManualDurabilityReduction : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;


protected:

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float Ammount = 10.f;

};
