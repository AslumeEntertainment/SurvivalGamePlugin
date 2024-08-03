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

	UPROPERTY()
	class USG_AnimInstance* AnimInst;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	bool bUsesItem = false;

	UPROPERTY()
	AItemBase* UsedItem = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackAnimationMontage;

	UFUNCTION()
	void OnAnimMontageCompleted();

	UFUNCTION()
	void OnAnimMontageBlendOut();

	UFUNCTION()
	void OnAnimMontageCanceled();

};
