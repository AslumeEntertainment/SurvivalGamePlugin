// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"

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

void USG_AbilitySystemComponent::ApplyDefaultEffectsToSelf()
{
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(GetOwner());
	
	if (IsValid(OnSpawnInvincibilityEffectClass))
	{
		FGameplayEffectSpecHandle OnSpawnInvincibilitySpecHandle = MakeOutgoingSpec(OnSpawnInvincibilityEffectClass, 1, EffectContext);
		ApplyGameplayEffectSpecToSelf(*OnSpawnInvincibilitySpecHandle.Data.Get());
	}
}
