// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SGGA_MirrorableAnimatedAbility.h"

#include "Headers/DamageGameplayEffectStruct.h"

#include "SGGA_BasicAttackAbility.generated.h"

class AInGameCharacter;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_BasicAttackAbility : public USGGA_MirrorableAnimatedAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	bool DoesAbilityUseItems();


protected:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTagContainer ActiveDamageComponentTags; 

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<FDamageGameplayEffect> AdditionalDamageEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<TSubclassOf<UGameplayEffect>, TSubclassOf<UGameplayEffect>> DamagingEffectPerPlayerEffectTable;

	AInGameCharacter* Player;

};
