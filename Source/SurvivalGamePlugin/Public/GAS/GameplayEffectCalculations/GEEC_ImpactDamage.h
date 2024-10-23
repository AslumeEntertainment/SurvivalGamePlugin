// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_ImpactDamage.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_ImpactDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UGEEC_ImpactDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> InvincibilityEffectClass;

};
