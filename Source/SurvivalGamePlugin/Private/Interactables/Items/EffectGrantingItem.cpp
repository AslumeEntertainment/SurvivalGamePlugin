// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/EffectGrantingItem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemGlobals.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"


FGameplayEffectSpecHandle AEffectGrantingItem::CreateEffectSpecHandle(int EffectID)
{
	FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(USG_AbilitySystemGlobals::Get().AllocGameplayEffectContext());
	EffectContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpecHandle = UAbilitySystemBlueprintLibrary::MakeSpecHandle(Effects[EffectID].GetDefaultObject(), GetOwner(), GetOwner());
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();

	EffectSpec->SetContext(EffectContextHandle);
	EffectSpec->SetLevel(EffectLevel);
	
	return EffectSpecHandle;
}

TArray<TSubclassOf<UGameplayEffect>> AEffectGrantingItem::GetEffects()
{
	return Effects;
}
