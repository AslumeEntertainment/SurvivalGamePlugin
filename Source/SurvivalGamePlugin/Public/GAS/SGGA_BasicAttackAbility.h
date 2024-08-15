// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SG_GameplayAbility.h"
#include "SGGA_BasicAttackAbility.generated.h"

class AItemBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_BasicAttackAbility : public USG_GameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	bool bUsesItem = false;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackAnimationMontage;

	UPROPERTY(EditDefaultsOnly, Category = "OnHitRegistered")
	FGameplayTag OnHitGameplayEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "OnHitRegistered")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	class ASG_Character* Player;
	class USG_AnimInstance* AnimInst;

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageBlendOut();

	UFUNCTION()
	void OnAnimMontageCanceled();

	UFUNCTION()
	void OnEventRecieved(FGameplayEventData Payload);

};
