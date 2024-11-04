// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_OnStartedBurning.generated.h"

class UPassiveDamageDealingComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_OnStartedBurning : public USG_GameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


protected:

	AActor* OwnerActor = nullptr;

	FTimerHandle FireSpreadTimer;

	UPassiveDamageDealingComponent* OwnerPassiveDamageDealer;
	 
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	bool bShouldAttemptSpreadOnActivation = true;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float Frequency = 1.f;

	/** This will be replaced when we set up the fire related gameplay attributes */
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues", meta = (ClampMin = "1", ClampMax = "100", UIMin = "1", UIMax = "100"))
	int ChanceToSpread = 50;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	TSubclassOf<UGameplayEffect> BurningEffect;

	UFUNCTION()
	void OnEffectEnded(FGameplayEventData Payload);

	UFUNCTION()
	void AttemptSpreadFire();
};
