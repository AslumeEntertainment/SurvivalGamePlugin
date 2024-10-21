// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_UnequipItem.generated.h"

class AInGameCharacter;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_UnequipItem : public USG_GameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:

	AInGameCharacter* Player;

	UFUNCTION()
	void UnequipMain(FGameplayEventData Payload);

	UFUNCTION()
	void UnequipSecondary(FGameplayEventData Payload);

	UFUNCTION()
	void UnequipMixed(FGameplayEventData Payload);

	UFUNCTION()
	void OnUnequipCanceled(float TimeHeld);
};
