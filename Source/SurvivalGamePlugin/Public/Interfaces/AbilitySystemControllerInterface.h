// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"

#include "AbilitySystemControllerInterface.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class USG_GameplayAbility;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAbilitySystemControllerInterface : public UInterface
{
	GENERATED_BODY()
};


class SURVIVALGAMEPLUGIN_API IAbilitySystemControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void InitASC(AActor* AvatarActor) = 0;

	virtual FActiveGameplayEffectHandle AddEffect(TSubclassOf<UGameplayEffect> GameplayEffect, uint8 Level = 1) = 0;

	virtual void RemoveEffect(FActiveGameplayEffectHandle RemovedEffectSpecHandle, int8 StacksToRemove = -1) = 0;

	virtual FGameplayAbilitySpecHandle AddAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, uint8 Level = 1) = 0;

	virtual void RemoveAbility(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle) = 0;
};
