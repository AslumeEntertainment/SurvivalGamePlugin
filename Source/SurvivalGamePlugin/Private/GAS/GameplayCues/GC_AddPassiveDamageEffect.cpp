// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_AddPassiveDamageEffect.h"

bool UGC_AddPassiveDamageEffect::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	return false;
}

bool UGC_AddPassiveDamageEffect::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	//UE_LOG(LogTemp, Warning, TEXT("NO BALLS?!"));
	return true;
}

bool UGC_AddPassiveDamageEffect::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	return false;
}
