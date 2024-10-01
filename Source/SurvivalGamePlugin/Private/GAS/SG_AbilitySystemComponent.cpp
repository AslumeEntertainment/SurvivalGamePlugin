// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemComponent.h"

#include "GameplayEffect.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/Abilities/SG_GameplayAbility.h"

void USG_AbilitySystemComponent::Server_SendGameplayEventToSelf_Implementation(FGameplayTag EventTag, FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_SendGameplayEventToSelf - %s"), *EventTag.ToString());
	HandleGameplayEvent(EventTag, &Payload);
}

void USG_AbilitySystemComponent::Client_SendGameplayEventToSelf_Implementation(FGameplayTag EventTag, FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Client_SendGameplayEventToSelf - %s"), *EventTag.ToString());
	Server_SendGameplayEventToSelf(EventTag, Payload);
}

FActiveGameplayEffectHandle USG_AbilitySystemComponent::AddEffectFromClass(TSubclassOf<UGameplayEffect> GameplayEffectClass, UObject* Instigator, uint8 Level)
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

void USG_AbilitySystemComponent::AddAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* Instigator)
{
	for (TPair<TSubclassOf<USG_GameplayAbility>, int> Ability : AbilityMap)
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability.Key, Ability.Value, static_cast<int32>((Ability.Key).GetDefaultObject()->AbilityInputID), Instigator);
		GiveAbility(Spec);
	}
}

void USG_AbilitySystemComponent::AddPassiveAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* Instigator)
{
	for (TPair<TSubclassOf<USG_GameplayAbility>, int> Ability : AbilityMap)
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability.Key, Ability.Value, static_cast<int32>((Ability.Key).GetDefaultObject()->AbilityInputID), Instigator);
		GiveAbilityAndActivateOnce(Spec);
	}
}

void USG_AbilitySystemComponent::RemoveAbilityFromSpec(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle)
{
	ClearAbility(RemovedAbilitySpecHandle);
}
