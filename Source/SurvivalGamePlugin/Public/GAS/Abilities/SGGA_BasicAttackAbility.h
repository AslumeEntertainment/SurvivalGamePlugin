// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SGGA_MirrorableAnimatedAbility.h"
#include "SGGA_BasicAttackAbility.generated.h"

class AInGameCharacter;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_BasicAttackAbility : public USGGA_MirrorableAnimatedAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<TSubclassOf<UGameplayEffect>, TSubclassOf<UGameplayEffect>> DamagingEffectPerPlayerEffectTable;

	AInGameCharacter* Player;

};
