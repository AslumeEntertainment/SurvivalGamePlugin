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

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void AbilityInputPressed(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void AbilityInputReleased(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	FActiveGameplayEffectHandle AddEffectFromClass(TSubclassOf<UGameplayEffect> GameplayEffectClass, const UObject* Instigator, uint8 Level = 1);

	void RemoveEffectFromSpec(FActiveGameplayEffectHandle GameplayEffectSpecHandle, int8 StackToRemove = -1);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void AddAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* Instigator);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void AddPassiveAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* Instigator);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void RemoveAbilityFromSpec(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle);

};
