// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_ApplyEffectsFromItemToSelf.generated.h"

class AInGameCharacter;
class AEffectGrantingItem;
class USG_AnimInstance;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_ApplyEffectsFromItemToSelf : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* UseAnimationMontage;

	UPROPERTY(EditDefaultsOnly, Category = "OnNotifyRecieved")
	FGameplayTag OnNotifyRecievedGameplayEventTag;

	AInGameCharacter* Player;
	AEffectGrantingItem* EffectGrantingItem;
	USG_AnimInstance* AnimInst;

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageCanceled();

	UFUNCTION()
	void OnEventRecieved(FGameplayEventData Payload);
	
};
