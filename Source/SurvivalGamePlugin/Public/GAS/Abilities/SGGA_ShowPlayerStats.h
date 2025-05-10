// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_ShowPlayerStats.generated.h"

class UInGameStatsWindow;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_ShowPlayerStats : public USG_GameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:

	UPROPERTY()
	UInGameStatsWindow* InGameStatsWindow;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInGameStatsWindow> InGameStatsWindowClass;

	UFUNCTION()
	void CloseWindow(float TimeHeld);

};
