// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "Headers/DamageGameplayEffectStruct.h"
#include "SGGA_BasicAttackAbility.generated.h"

class AInGameCharacter;
class USG_AnimInstance;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_BasicAttackAbility : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	FGameplayTagContainer GetMirroredAbilityDamageComponentTags();

protected:

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (ToolTip = "Should this ability's animation be mirrored when used by a different hand"))
	bool bCanBeMirrored = false;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackAnimationMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTagContainer ActiveDamageComponentTags; 

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<FDamageGameplayEffect> AdditionalDamageEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<TSubclassOf<UGameplayEffect>, TSubclassOf<UGameplayEffect>> DamagingEffectPerPlayerEffectTable;

	AInGameCharacter* Player;

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageBlendOut();

	UFUNCTION()
	void OnAnimMontageCanceled();

};
