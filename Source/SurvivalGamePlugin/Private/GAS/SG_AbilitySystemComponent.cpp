// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemComponent.h"

#include "GameplayEffect.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/Abilities/SG_GameplayAbility.h"


void USG_AbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::AbilityInputPressed InputTag.IsValid() == false"));
		return;
	}

	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTag(InputTag) == false)
		{
			continue;
		}

		if (Spec.IsActive() == false)
		{
			TryActivateAbility(Spec.Handle);
		}
		else
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void USG_AbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::AbilityInputPressed InputTag.IsValid() == false"));
		return;
	}

	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTag(InputTag) == false)
		{
			continue;
		}

		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

FActiveGameplayEffectHandle USG_AbilitySystemComponent::AddEffectFromClass(TSubclassOf<UGameplayEffect> GameplayEffectClass, const UObject* Instigator, uint8 Level)
{
	FActiveGameplayEffectHandle GEHandle;
	
	if (IsValid(GameplayEffectClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::AddEffectFromClass IsValid(GameplayEffectClass) == false"));
		return GEHandle;
	}

	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(Instigator);

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffectClass, Level, EffectContext);

	if (EffectSpecHandle.IsValid())
	{
		GEHandle = ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}

	return GEHandle;
}

void USG_AbilitySystemComponent::RemoveEffectFromSpec(FActiveGameplayEffectHandle GameplayEffectSpecHandle, int8 StacksToRemove)
{
	RemoveActiveGameplayEffect(GameplayEffectSpecHandle, StacksToRemove);
}

FGameplayAbilitySpecHandle USG_AbilitySystemComponent::AddAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, int Level, UObject* Instigator, bool bAllowDuplicates)
{
	if (bAllowDuplicates == false)
	{
		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (Spec.Ability.GetClass() == AbilityClass)
			{
				return FGameplayAbilitySpecHandle();
			}
		}
	}
	FGameplayAbilitySpec Spec = FGameplayAbilitySpec(AbilityClass, Level, INDEX_NONE, Instigator);

	if (const USG_GameplayAbility* SG_Ability = Cast<USG_GameplayAbility>(Spec.Ability))
	{
		Spec.DynamicAbilityTags.AddTag(SG_Ability->InputTag);
		return GiveAbility(Spec);
	}

	return FGameplayAbilitySpecHandle();
}

void USG_AbilitySystemComponent::AddAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* Instigator, bool bAllowDuplicates)
{
	for (const TPair<TSubclassOf<USG_GameplayAbility>, int> Ability : AbilityMap)
	{
		AddAbility(Ability.Key, Ability.Value, Instigator, bAllowDuplicates);
	}
}

FGameplayAbilitySpecHandle USG_AbilitySystemComponent::AddPassiveAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, int Level, UObject* Instigator, bool bAllowDuplicates)
{
	if (bAllowDuplicates == false)
	{
		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (Spec.Ability.GetClass() == AbilityClass)
			{
				return FGameplayAbilitySpecHandle();
			}
		}
	}
	
	FGameplayAbilitySpec Spec = FGameplayAbilitySpec(AbilityClass, Level, INDEX_NONE, Instigator);
	return GiveAbilityAndActivateOnce(Spec);
}

void USG_AbilitySystemComponent::AddPassiveAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* Instigator, bool bAllowDuplicates)
{
	for (TPair<TSubclassOf<USG_GameplayAbility>, int> Ability : AbilityMap)
	{
		AddPassiveAbility(Ability.Key, Ability.Value, Instigator, bAllowDuplicates);
	}
}

void USG_AbilitySystemComponent::RemoveAbilityFromSpec(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle)
{
	ClearAbility(RemovedAbilitySpecHandle);
}


