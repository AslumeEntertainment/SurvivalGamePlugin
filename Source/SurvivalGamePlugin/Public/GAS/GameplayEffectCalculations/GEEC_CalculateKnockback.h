// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_CalculateKnockback.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_CalculateKnockback : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:

	FVector GetHitDirection(FVector HitLocation, FVector TargetCenter) const;

};
