// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_OnEnergyDrained.generated.h"

class UAbilitySystemComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_OnEnergyDrained : public USG_GameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UAbilitySystemComponent* ASC;

	UPROPERTY(EditDefaultsOnly)
	float SpeedMultiplier;

	UPROPERTY(EditDefaultsOnly)
	float AttackSpeedMultiplier;

	UFUNCTION()
	void OnEnergyRecharged(FGameplayEventData Payload);
};
