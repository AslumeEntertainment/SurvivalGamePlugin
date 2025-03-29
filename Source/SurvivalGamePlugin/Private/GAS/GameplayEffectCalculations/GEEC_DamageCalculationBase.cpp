// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculationBase.h"

#include "Objects/GameplayEffectDataObjects/DamageSourceData.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/DamageDealingItem.h"
#include "Headers/PhysicalPropertyEnums.h"


bool UGEEC_DamageCalculationBase::IsTargetImmuneToDamageType(USG_AbilitySystemComponent* TargetASC, ESG_DamageType DamageType) const
{
	for (TPair<FGameplayTag, ESG_DamageType> DamageImmunityEntry : DamageImmunityForTagsOwned)
	{
		if (TargetASC->HasMatchingGameplayTag(DamageImmunityEntry.Key) == false)
		{
			continue;
		}
		if (DamageImmunityEntry.Value == DamageType)
		{
			return true;
		}
	}
	return false;
}

void UGEEC_DamageCalculationBase::TriggerGameplayCues(USG_AbilitySystemComponent* TargetASC, FGameplayCueParameters CueParams) const
{
	if (bShouldTriggerCuesOnZeroMagnitude == false && CueParams.RawMagnitude == 0.f)
	{
		return;
	}

	for (FGameplayTag GameplayCueTag : GameplayCuesToExecuteOnSuccess)
	{
		TargetASC->ExecuteGameplayCue(GameplayCueTag, CueParams);
	}
}

void UGEEC_DamageCalculationBase::ApplyAdditionalEffects(USG_AbilitySystemComponent* TargetASC, TArray<FAdditionalEffectDataStruct> AdditionalEffectsData, FGameplayEffectContextHandle EffectContext, float InstigatorDexterity) const
{
	int RandInt = FMath::RandRange(1, 100) + InstigatorDexterity;

	FGameplayEffectContextHandle NewEffectContext = EffectContext;

	for (FAdditionalEffectDataStruct AdditionalEffect : AdditionalEffectsData)
	{
		if (RandInt > AdditionalEffect.ChanceToApply)
		{
			continue;
		}
		if (IsValid(AdditionalEffect.AdditionalEffectData) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculationBase::Execute_Implementation IsValid(AdditionalEffect.AdditionalEffectData) == false"));
			continue;
		}
		if (IsValid(AdditionalEffect.AdditionalEffectData->EffectClass) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculationBase::Execute_Implementation IsValid(AdditionalEffect.AdditionalEffectData->EffectClass) == false"));
			continue;
		}

		NewEffectContext.AddSourceObject(AdditionalEffect.AdditionalEffectData);

		FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(AdditionalEffect.AdditionalEffectData->EffectClass, AdditionalEffect.AdditionalEffectData->EffectLevel, NewEffectContext);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UGEEC_DamageCalculationBase::AcknowledgeHit(USG_AbilitySystemComponent* InstigatorASC, USG_AbilitySystemComponent* TargetASC, FGameplayEventData EventData) const
{
	if (IsValid(InstigatorASC))
	{
		InstigatorASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Triggered"), &EventData);
	}
	if (IsValid(TargetASC))
	{
		TargetASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Received"), &EventData);
	}
}
