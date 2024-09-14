// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SG_AbilitySystemComponent.generated.h"

class UGameplayEffect;

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "DefaultGameplayEffects")
	TSubclassOf<UGameplayEffect> OnSpawnInvincibilityEffectClass;

public:

	virtual void ApplyDefaultEffectsToSelf();

};
