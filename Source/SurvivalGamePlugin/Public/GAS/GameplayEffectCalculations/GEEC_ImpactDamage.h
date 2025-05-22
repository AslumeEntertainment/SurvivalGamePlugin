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

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues", meta = (ClampMin = "-1.0", ClampMax = "0.0", UIMin = "-1.0", UIMax = "0.0"))
	float ZVelocityFallAngle = -0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float FallDamageMultiplier = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float HorizontalDamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float MininumDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float InternalImpactDamageThresholdAngle = 95.f;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float ExternalImpactDamageThresholdAngle = 75.f;

	float CalculateSharedImpactDamage(AActor* ActorBeeingHit, AActor* HittingActor) const;

	float CalculateInternalImpactDamage(AActor* TargetActor, const FHitResult* HitResult) const;

	float CalculateExternalImpactDamage(AActor* TargetActor, const FHitResult* HitResult) const;
};
