// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemGlobals.h"
#include "Headers/DamageGameplayEffectStruct.h"

FGameplayEffectContext* USG_AbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FSG_GameplayEffectContext();
}
