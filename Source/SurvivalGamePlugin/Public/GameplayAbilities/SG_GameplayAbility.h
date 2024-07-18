// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Headers/GameplayAbilityInputID.h"
#include "SG_GameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API USG_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	USG_GameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGameplayAbilityInputID AbilityInputID = EGameplayAbilityInputID::None;
};
