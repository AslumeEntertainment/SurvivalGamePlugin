// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_AttributeSet.h"

void USG_AbilitySystemComponent::ApplyDefaultEffectsToSelf()
{
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(GetOwner());

	if (IsValid(HealthRegenerationEffectClass))
	{
		FGameplayEffectSpecHandle HealthRegenerationSpecHandle = MakeOutgoingSpec(HealthRegenerationEffectClass, 1, EffectContext);
		FGameplayEffectSpec* HealthRegenerationSpec = HealthRegenerationSpecHandle.Data.Get();
		//HealthRegenerationSpec->
		HealthRegenerationSpec->Period = GetNumericAttribute(USG_AttributeSet::GetHealthRegenerationRateAttribute());
		HealthRegenerationEffect = ApplyGameplayEffectSpecToSelf(*HealthRegenerationSpec);
	}
	
	if (IsValid(HungerDrainEffectClass))
	{
		FGameplayEffectSpecHandle HungerDrainSpecHandle = MakeOutgoingSpec(HungerDrainEffectClass, 1, EffectContext);
		FGameplayEffectSpec* HungerDrainSpec = HungerDrainSpecHandle.Data.Get();
		HungerDrainSpec->Period = GetNumericAttribute(USG_AttributeSet::GetHungerDrainRateAttribute());
		HungerDrainEffect = ApplyGameplayEffectSpecToSelf(*HungerDrainSpec);
	}
}

void USG_AbilitySystemComponent::SetHealthRegenerationRate(float NewValue)
{
	//HealthRegenerationEffect
}

void USG_AbilitySystemComponent::SetHungerDrainRate(float NewValue)
{
	//
}
