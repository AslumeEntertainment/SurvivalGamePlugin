// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculationBase.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/DamageDealingItem.h"
#include "Headers/PhysicalPropertyEnums.h"
#include "Objects/DamageSourceData.h"


float UGEEC_DamageCalculationBase::GetStrengthMultiplier(USG_AbilitySystemComponent* InstigatorASC, AActor* EffectCauser) const
{
	float StrengthMultiplier = 1.f;

	if (IsValid(InstigatorASC) == false)
	{
		return StrengthMultiplier;
	}
	if (IsValid(EffectCauser) == false)
	{
		return StrengthMultiplier;
	}

	EWeight EffectCauserWeight = EWeight::Normal;
	if (IsValid(Cast<ADamageDealingItem>(EffectCauser)))
	{
		EffectCauserWeight = Cast<ADamageDealingItem>(EffectCauser)->ItemData.ItemWeight;
	}

	StrengthMultiplier = InstigatorASC->GetNumericAttribute(USG_CharacterAttributeSet::GetStrengthAttribute()) / (uint8)EffectCauserWeight;

	return StrengthMultiplier;
}

float UGEEC_DamageCalculationBase::GetPenetrationMultiplier(const UDamageSourceData* DamageSourceData, float InstigatorVelocity, USG_AbilitySystemComponent* TargetASC, FVector HitLocation) const
{
	if (IsTargetImmuneToDamageType(TargetASC, DamageSourceData->DamageType))
	{
		return 0.f;
	}

	float PenetrationMultiplier = 1.f;

	IHittable* HittableTarget = Cast<IHittable>(TargetASC->GetOwner());
	if (HittableTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculationBase::GetPenetrationMultiplier HittableTarget == nullptr"));
		return PenetrationMultiplier;
	}

	float TargetResistance = 1.f;

	switch (DamageSourceData->DamageType)
	{
	case ESG_DamageType::Slashing: TargetResistance = TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistanceSlashingAttribute()); break;
	case ESG_DamageType::Piercing: TargetResistance = TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistancePiercingAttribute()); break;
	case ESG_DamageType::Bludgeoning: TargetResistance = TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistanceBludgeoningAttribute()); break;
	case ESG_DamageType::Fire: TargetResistance = TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistanceFireAttribute());
	}

	if (TargetResistance <= 0) TargetResistance = 1;

	if (HittableTarget->IsHitGuarded(HitLocation))
	{
		UE_LOG(DamageLog, Warning, TEXT("The hit is being guarded by the target"));
		TargetResistance *= TargetASC->GetNumericAttribute(USG_AttributeSet::GetGuardProficiencyAttribute());
	}

	PenetrationMultiplier = (DamageSourceData->Penetration + DamageSourceData->Penetration * InstigatorVelocity / 100) / TargetResistance;

	return PenetrationMultiplier;
}

bool UGEEC_DamageCalculationBase::IsTargetImmuneToDamageType(USG_AbilitySystemComponent* TargetASC, ESG_DamageType DamageType) const
{
	for (TPair<FGameplayTag,ESG_DamageType> DamageImmunityEntry : DamageImmunityForTagsOwned)
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
		if (IsValid(AdditionalEffect.AdditionalEffectData->DamageEffect) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculationBase::Execute_Implementation IsValid(AdditionalEffect.AdditionalEffectData->DamageEffect) == false"));
			continue;
		}

		NewEffectContext.AddSourceObject(AdditionalEffect.AdditionalEffectData);

		FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(AdditionalEffect.AdditionalEffectData->DamageEffect, AdditionalEffect.AdditionalEffectData->EffectLevel, NewEffectContext);
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
