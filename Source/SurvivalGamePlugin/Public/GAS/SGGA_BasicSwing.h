// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SG_GameplayAbility.h"
#include "SGGA_BasicSwing.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_BasicSwing : public USG_GameplayAbility
{
	GENERATED_BODY()
	
protected:

	UPROPERTY()
	class USG_AnimInstance* AnimInst;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AnimationMontage;

	UFUNCTION()
	void OnMontageBlendOut();

	UFUNCTION()
	void OnMontageCompleted();

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
