// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Headers/GeneralDelegates.h"
#include "SG_AbilitySystemComponent.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class USG_GameplayAbility;
class ADamageIndicator;

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystemComponentDefaults")
	TMap<FGameplayAttribute, float> StartingAttributeSetData;

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystemComponentDefaults", meta = (Categories = "GameplayCue"))
	FGameplayTag OnDestroyedGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystemComponentDefaults")
	TSubclassOf<ADamageIndicator> DamageIndicatorClass = nullptr;


public:

	UFUNCTION()
	void OnOwnerDestroyed();

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystemComponentDefaults")
	bool bCanEverReceiveDamageFromEffects = true;

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystemComponentDefaults")
	bool bCanDisplayDamageIndicator = true;

	UFUNCTION(BlueprintCallable)
	void SetStartingAttributeValues();

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void AbilityInputPressed(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void AbilityInputReleased(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	FActiveGameplayEffectHandle AddEffectFromClass(TSubclassOf<UGameplayEffect> GameplayEffectClass, const UObject* SourceObject, AActor* Instigator, AActor* EffectCauser, uint8 Level = 1);

	void RemoveEffectFromSpec(FActiveGameplayEffectHandle GameplayEffectSpecHandle, int8 StackToRemove = -1);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	FGameplayAbilitySpecHandle AddAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, int Level, UObject* SourceObject, bool bAllowDuplicates = true);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void AddAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* SourceObject, bool bAllowDuplicates = true);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	FGameplayAbilitySpecHandle AddPassiveAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, int Level, UObject* SourceObject, bool bAllowDuplicates = true);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void AddPassiveAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* SourceObject, bool bAllowDuplicates = true);

	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void RemoveAbilityFromSpec(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle);

	void GetActivatableGameplayAbilitySpecFromClass(TSubclassOf<UGameplayAbility> AbilityClass, struct FGameplayAbilitySpec*& MatchingGameplayAbility) const;

	void GetActivatableGameplayAbilitySpecsFromType(FGameplayTag AbilityTypeTag, TArray <struct FGameplayAbilitySpec*>& MatchingGameplayAbilities) const;

};
