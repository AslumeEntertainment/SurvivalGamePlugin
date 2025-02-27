// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SG_PassiveGameplayAbility.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_PassiveGameplayAbility : public USG_GameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:

	UAbilityTask_WaitGameplayEvent* OnHitTriggeredEventTask;

	UAbilityTask_WaitGameplayEvent* OnHitReceivedEventTask;

	UPROPERTY(EditDefaultsOnly, Category = "PassiveAbilityDefaults")
	bool bShouldTriggerOnHitDealt = true;

	UPROPERTY(EditDefaultsOnly, Category = "PassiveAbilityDefaults")
	bool bShouldTriggerOnHitReceived = true;

	UFUNCTION()
	virtual void OnHitEventReceived(FGameplayEventData Payload) PURE_VIRTUAL(USG_PassiveGameplayAbility::OnHitEventReceived);

	UFUNCTION()
	virtual void OnHitEventDealt(FGameplayEventData Payload) PURE_VIRTUAL(USG_PassiveGameplayAbility::OnHitEventDealt);

};
