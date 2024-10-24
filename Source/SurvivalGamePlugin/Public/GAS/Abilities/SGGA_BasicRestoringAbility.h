// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_BasicRestoringAbility.generated.h"

class AInGameCharacter;
class ARegeneratingItem;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_BasicRestoringAbility : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackAnimationMontage;

	UPROPERTY(EditDefaultsOnly, Category = "OnNotifyRecieved")
	FGameplayTag OnNotifyRecievedGameplayEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "OnNotifyRecieved")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "OnNotifyRecieved")
	float EffectDuration;

	UPROPERTY(EditDefaultsOnly, Category = "OnNotifyRecieved")
	float EffectPeriod;

	AInGameCharacter* Player;

	ARegeneratingItem* RegeneratingItem;

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageCanceled();

	UFUNCTION()
	void OnEventRecieved(FGameplayEventData Payload);
	
};
