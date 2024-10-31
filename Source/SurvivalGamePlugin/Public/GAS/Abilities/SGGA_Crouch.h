// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_Crouch.generated.h"

class ASG_Character;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_Crouch : public USG_GameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);


protected:

	FActiveGameplayEffectHandle JumpVelocityDecreaseEffectSpecHandle;

	ASG_Character* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Speed")
	TSubclassOf<UGameplayEffect> JumpVelocityDecreaseEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Speed")
	float JumpVelocityMultiplier;

	UFUNCTION()
	void OnCrouchReleased(float TimeHeld);
};
