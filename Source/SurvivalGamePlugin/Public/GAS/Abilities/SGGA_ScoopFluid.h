// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SGGA_MirrorableAnimatedAbility.h"
#include "SGGA_ScoopFluid.generated.h"

class AInGameCharacter;
class AFluidContainerItem;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_ScoopFluid : public USGGA_MirrorableAnimatedAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:

	AInGameCharacter* Player;

	AFluidContainerItem* FluidContainerItem;

	UPROPERTY(EditDefaultsOnly, Category = "OnNotifyRecieved")
	FGameplayTag OnNotifyRecievedGameplayEventTag;

	UFUNCTION()
	void OnEventRecieved(FGameplayEventData Payload);
};
