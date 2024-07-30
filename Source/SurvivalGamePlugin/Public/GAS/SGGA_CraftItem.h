// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SG_GameplayAbility.h"
#include "SGGA_CraftItem.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_CraftItem : public USG_GameplayAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	UDataTable* CraftingInfoTable;

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
