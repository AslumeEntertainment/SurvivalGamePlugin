// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AttributeSet.h"
#include "Net/UnrealNetwork.h"

USG_AttributeSet::USG_AttributeSet()
{
}

void USG_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, DamageRight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, DamageLeft, COND_None, REPNOTIFY_Always);
}

void USG_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void USG_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void USG_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Health, OldHealth);
}

void USG_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxHealth, OldMaxHealth);
}

void USG_AttributeSet::OnRep_DamageRight(const FGameplayAttributeData& OldDamageRight)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, DamageRight, OldDamageRight);
}

void USG_AttributeSet::OnRep_DamageLeft(const FGameplayAttributeData& OldDamageLeft)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, DamageLeft, OldDamageLeft);
}

