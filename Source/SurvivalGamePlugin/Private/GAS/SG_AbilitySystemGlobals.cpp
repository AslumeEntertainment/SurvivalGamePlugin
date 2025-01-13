// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemGlobals.h"

#include "Headers/DamageGameplayEffectStruct.h"
#include "GAS/SG_GameplayEffectContext.h"

FGameplayEffectContext* USG_AbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FSG_GameplayEffectContext();
}
