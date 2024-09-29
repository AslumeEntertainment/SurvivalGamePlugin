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

FActiveGameplayEffectHandle USG_AbilitySystemComponent::AddEffectFromClass(TSubclassOf<UGameplayEffect> GameplayEffectClass, uint8 Level)
{
	FActiveGameplayEffectHandle GEHandle;
	
	if (IsValid(GameplayEffectClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::AddEffectFromClass IsValid(GameplayEffectClass) == false"));
		return GEHandle;
	}

	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(this);

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

FGameplayAbilitySpecHandle USG_AbilitySystemComponent::AddAbilityFromClass(TSubclassOf<USG_GameplayAbility> AbilityClass, uint8 Level)
{
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	if (IsValid(AbilityClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::AddAbilityFromClass IsValid(AbilityClass) == false"));
		return AbilitySpecHandle;
	}

	AbilitySpecHandle = GiveAbility(FGameplayAbilitySpec(AbilityClass, Level, static_cast<int32>(AbilityClass.GetDefaultObject()->AbilityInputID), this));
	
	return AbilitySpecHandle;
}

void USG_AbilitySystemComponent::RemoveAbilityFromSpec(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle)
{
	ClearAbility(RemovedAbilitySpecHandle);
}
