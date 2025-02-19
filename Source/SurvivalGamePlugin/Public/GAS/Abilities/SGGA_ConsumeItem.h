// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SGGA_MirrorableAnimatedAbility.h"

#include "GameplayTagContainer.h"

#include "SGGA_ConsumeItem.generated.h"

class AInGameCharacter;
class AConsumableItem;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_ConsumeItem : public USGGA_MirrorableAnimatedAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:

	UPROPERTY(EditDefaultsOnly, Category = "OnNotifyRecieved")
	FGameplayTag OnNotifyRecievedGameplayEventTag;

	AInGameCharacter* Player;

	AConsumableItem* ConsumableItem;

	UFUNCTION()
	void OnEventRecieved(FGameplayEventData Payload);
};
