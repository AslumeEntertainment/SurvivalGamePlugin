// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"

void USG_AbilitySystemComponent::ApplyDefaultEffectsToSelf()
{
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(GetOwner());

	if (IsValid(HealthRegenerationEffectClass))
	{
		FGameplayEffectSpecHandle HealthRegenerationSpecHandle = MakeOutgoingSpec(HealthRegenerationEffectClass, 1, EffectContext);
		FGameplayEffectSpec* HealthRegenerationSpec = HealthRegenerationSpecHandle.Data.Get();
		HealthRegenerationSpec->Period = GetNumericAttribute(USG_CharacterAttributeSet::GetHealthRegenerationRateAttribute());
		HealthRegenerationEffect = ApplyGameplayEffectSpecToSelf(*HealthRegenerationSpec);
	}
	
	if (IsValid(HungerDrainEffectClass))
	{
		FGameplayEffectSpecHandle HungerDrainSpecHandle = MakeOutgoingSpec(HungerDrainEffectClass, 1, EffectContext);
		FGameplayEffectSpec* HungerDrainSpec = HungerDrainSpecHandle.Data.Get();
		HungerDrainSpec->Period = GetNumericAttribute(USG_CharacterAttributeSet::GetHungerDrainRateAttribute());
		HungerDrainEffect = ApplyGameplayEffectSpecToSelf(*HungerDrainSpec);
	}

	if (IsValid(EnergyRechargeEffectClass))
	{
		FGameplayEffectSpecHandle EnergyRechargeSpecHandle = MakeOutgoingSpec(EnergyRechargeEffectClass, 1, EffectContext);
		FGameplayEffectSpec* EnergyRechargeSpec = EnergyRechargeSpecHandle.Data.Get();
		EnergyRechargeSpec->Period = GetNumericAttribute(USG_CharacterAttributeSet::GetEnergyRechargeRateAttribute());
		EnergyRechargeEffect = ApplyGameplayEffectSpecToSelf(*EnergyRechargeSpec);
	}

	if (IsValid(EnergyDrainEffectClass))
	{
		FGameplayEffectSpecHandle EnergyDrainSpecHandle = MakeOutgoingSpec(EnergyDrainEffectClass, 1, EffectContext);
		FGameplayEffectSpec* EnergyDrainSpec = EnergyDrainSpecHandle.Data.Get();
		EnergyDrainSpec->Period = GetNumericAttribute(USG_CharacterAttributeSet::GetEnergyDrainRateAttribute());
		EnergyDrainEffect = ApplyGameplayEffectSpecToSelf(*EnergyDrainSpec);
	}
}

void USG_AbilitySystemComponent::SetHealthRegenerationRate(float NewValue)
{
	//Functionality to be added
}

void USG_AbilitySystemComponent::SetHungerDrainRate(float NewValue)
{
	//Functionality to be added
}

void USG_AbilitySystemComponent::SetEnergyRechargeRate(float NewValue)
{
	//Functionality to be added
}

void USG_AbilitySystemComponent::SetEnergyDrainRate(float NewValue)
{
	//Functionality to be added
}
