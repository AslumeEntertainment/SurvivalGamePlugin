// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_CalculateDamage.generated.h"

class AActor;
class UDamageSourceData;
class USG_AbilitySystemComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API UGEEC_CalculateDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UGEEC_CalculateDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:

	UPROPERTY(EditAnywhere)
	bool bShouldPrintDebug = true;

	UPROPERTY(EditAnywhere)
	FGameplayTag DamageIndicatorGameplayCueTag;

	float GetStrengthMultiplier(USG_AbilitySystemComponent* InstigatorASC, AActor* EffectCauser) const;

	float GetInstigatorVelocity_MetersPerSec(AActor* InstigatorActor) const;

	float GetPenetrationMultiplier(const UDamageSourceData* DamageSourceData, float InstigatorVelocity, USG_AbilitySystemComponent* TargetASC, FVector HitLocation) const;

};
