// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SG_AbilitySystemComponent.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "DefaultGameplayEffects")
	TSubclassOf<class UGameplayEffect> HealthRegenerationEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultGameplayEffects")
	TSubclassOf<class UGameplayEffect> HungerDrainEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultGameplayEffects")
	TSubclassOf<class UGameplayEffect> EnergyRechargeEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultGameplayEffects")
	TSubclassOf<class UGameplayEffect> EnergyDrainEffectClass;

	UPROPERTY()
	FActiveGameplayEffectHandle HealthRegenerationEffect;

	UPROPERTY()
	FActiveGameplayEffectHandle HungerDrainEffect;

	UPROPERTY()
	FActiveGameplayEffectHandle EnergyRechargeEffect;

	UPROPERTY()
	FActiveGameplayEffectHandle EnergyDrainEffect;
	
public:

	void ApplyDefaultEffectsToSelf();

	UFUNCTION()
	void SetHealthRegenerationRate(float NewValue);

	UFUNCTION()
	void SetHungerDrainRate(float NewValue);

	UFUNCTION()
	void SetEnergyRechargeRate(float NewValue);

	UFUNCTION()
	void SetEnergyDrainRate(float NewValue);
};
