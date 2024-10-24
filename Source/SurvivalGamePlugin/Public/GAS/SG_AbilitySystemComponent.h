// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SG_AbilitySystemComponent.generated.h"

class UGameplayEffect;
class USG_GameplayAbility;

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	void AbilityInputPressed(const FGameplayTag& InputTag);

	void AbilityInputReleased(const FGameplayTag& InputTag);

	FActiveGameplayEffectHandle AddEffectFromClass(TSubclassOf<UGameplayEffect> GameplayEffectClass, const AActor* Instigator, uint8 Level = 1);

	void RemoveEffectFromSpec(FActiveGameplayEffectHandle GameplayEffectSpecHandle, int8 StackToRemove = -1);

	void AddAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* Instigator);

	void AddPassiveAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* Instigator);

	void RemoveAbilityFromSpec(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle);

};
