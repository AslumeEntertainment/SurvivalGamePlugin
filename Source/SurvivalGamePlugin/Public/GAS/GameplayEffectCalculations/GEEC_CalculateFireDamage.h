// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Headers/DamageGameplayEffectStruct.h"
#include "GEEC_CalculateFireDamage.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_CalculateFireDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UGEEC_CalculateFireDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag SetByCallerMagnitudeDataTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag SetByCallerMagnitudeEffectDurationTag;

	UPROPERTY(EditAnywhere, Category = "DamageEffect")
	FDamageGameplayEffect DefaultBurningEffect;
};
