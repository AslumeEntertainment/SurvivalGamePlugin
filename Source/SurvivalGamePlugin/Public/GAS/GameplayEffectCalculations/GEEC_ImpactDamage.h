// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculationBase.h"
#include "GEEC_ImpactDamage.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_ImpactDamage : public UGEEC_DamageCalculationBase
{
	GENERATED_BODY()
	
public:

	UGEEC_ImpactDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float InternalImpactDamageThresholdAngle = 95.f;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float ExternalImpactDamageThresholdAngle = 75.f;

	float CalculateInternalImpactDamage(AActor* TargetActor, const FHitResult* HitResult) const;

	float CalculateExternalImpactDamage(AActor* TargetActor, const FHitResult* HitResult) const;
};
