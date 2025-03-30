// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculation_Standard.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Objects/GameplayEffectDataObjects/DamageSourceData.h"
#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/DamageDealingItem.h"
#include "Components/DamageDealingComponent.h"
#include "Headers/PhysicalPropertyEnums.h"
#include "Characters/InGameCharacter.h"

DEFINE_LOG_CATEGORY(DamageLog);

UGEEC_DamageCalculation_Standard::UGEEC_DamageCalculation_Standard()
{
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DexterityDef);

	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().GuardProficiencyDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceSlashingDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistancePiercingDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceBludgeoningDef);
}

void UGEEC_DamageCalculation_Standard::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FGameplayEffectContextHandle EffectContext = ExecutionParams.GetOwningSpec().GetEffectContext();

	AActor* InstigatorActor = EffectContext.GetInstigator();
	USG_AbilitySystemComponent* InstigatorASC = IsValid(InstigatorActor) ? Cast<USG_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor)) : nullptr;
	
	USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Standard::Execute_Implementation IsValid(TargetASC) == false"));
		return;
	}

	AActor* TargetActor = TargetASC->GetOwner();
	if (IsValid(TargetActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Standard::Execute_Implementation IsValid(TargetActor) == false"));
		return;
	}

	IHittable* HittableTarget = Cast<IHittable>(TargetActor);
	if (HittableTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Standard::Execute_Implementation HittableTarget == nullptr"));
		return;
	}

	const UDamageSourceData* DamageSourceData = Cast<UDamageSourceData>(EffectContext.GetSourceObject());
	if (IsValid(DamageSourceData) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Standard::Execute_Implementation IsValid(DamageSourceData) == false"));
		return;
	}

	AActor* EffectCauser = EffectContext.GetEffectCauser();
	if (IsValid(EffectCauser) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Standard::Execute_Implementation IsValid(EffectCauser) == false"));
		return;
	}

	const FHitResult* HitResult = EffectContext.GetHitResult();
	/*if (HitResult == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Standard::Execute_Implementation HitResult == nullptr"));
		return;
	}*/

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

	//-------------DAMAGE-FORMULA-(IN-DEVELOPMENT)-------------//
	
	FVector HitLocation = (HitResult != nullptr) ? FVector(HitResult->ImpactPoint) : TargetActor->GetActorLocation();

	float FinalDamage = 0.f;
	
	if (IsTargetImmuneToDamageType(TargetASC, DamageSourceData->DamageType) == false)
	{
		float StrengthMultiplier = GetStrengthMultiplier(ExecutionParams, EvaluateParameters, EffectCauser);
		float InstigatorVelocity = IsValid(InstigatorActor) ? InstigatorActor->GetVelocity().Length() / 100 : 0;
		float PenetrationMultiplier = GetPenetrationMultiplier(ExecutionParams, EvaluateParameters, DamageSourceData, InstigatorVelocity, HittableTarget->IsHitGuarded(HitLocation));

		FinalDamage = FMath::Floor(DamageSourceData->BaseDamage * StrengthMultiplier * PenetrationMultiplier);

		if (bShouldPrintDebug)
		{
			UE_LOG(DamageLog, Log, TEXT("%s deals damage to %s using %s"), *InstigatorActor->GetName(), *TargetActor->GetName(), *EffectCauser->GetName());
			UE_LOG(DamageLog, Log, TEXT("Damage Type - %d (Slashing, Piercing, Bludgeoning, Fire)"), (uint8)DamageSourceData->DamageType);
			UE_LOG(DamageLog, Log, TEXT("StrengthMultiplier - %.2f"), StrengthMultiplier);
			UE_LOG(DamageLog, Log, TEXT("InstigatorVelocity - %.2f m/s"), InstigatorVelocity);
			UE_LOG(DamageLog, Log, TEXT("PenetrationMultiplier - %.2f"), PenetrationMultiplier);
			UE_LOG(DamageLog, Log, TEXT("FinalDamage - %.f"), FinalDamage);
		}
	}
	else
	{
		if (bShouldPrintDebug)
		{
			UE_LOG(DamageLog, Log, TEXT("%s deals damage to %s using %s"), *InstigatorActor->GetName(), *TargetActor->GetName(), *EffectCauser->GetName());
			UE_LOG(DamageLog, Log, TEXT("Damage Type - %d (Slashing, Piercing, Bludgeoning, Fire)"), (uint8)DamageSourceData->DamageType);
			UE_LOG(DamageLog, Log, TEXT("Target is immune to damage type, 0 damage dealt"));
		}
	}
	
	//---------------------------------------------------------//

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -FinalDamage));
	OutExecutionOutput.MarkConditionalGameplayEffectsToTrigger();

	float Dexterity = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DexterityDef, EvaluateParameters, Dexterity);

	ApplyAdditionalEffects(TargetASC, DamageSourceData->AdditionalEffects, EffectContext, Dexterity);

	FGameplayCueParameters CueParams = FGameplayCueParameters();
	CueParams.Location = DamageSourceData->bUseHitResultImpactLocation ? HitLocation : TargetActor->GetActorLocation();
	CueParams.EffectContext = EffectContext;
	CueParams.SourceObject = DamageSourceData;
	CueParams.RawMagnitude = FinalDamage;

	TriggerGameplayCues(TargetASC, CueParams);

	if (DamageSourceData->bShouldTriggerHitResponce == false)
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	EventData.Instigator = InstigatorActor;
	EventData.ContextHandle = EffectContext;

	AcknowledgeHit(InstigatorASC, TargetASC, EventData);

	return;

}

float UGEEC_DamageCalculation_Standard::GetStrengthMultiplier(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters EvaluateParameters, const AActor* EffectCauser) const
{
	float StrengthMultiplier = 1.f;

	float Strength = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvaluateParameters, Strength);
	if (Strength <= 0.f)
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

	StrengthMultiplier = Strength / (uint8)EffectCauserWeight;

	return StrengthMultiplier;
}

float UGEEC_DamageCalculation_Standard::GetPenetrationMultiplier(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters EvaluateParameters, const UDamageSourceData* DamageSourceData, float InstigatorVelocity, bool bIsHitGuarded) const
{
	float PenetrationMultiplier = 1.f;

	float TargetResistance = 1.f;

	switch (DamageSourceData->DamageType)
	{
	case ESG_DamageType::Slashing: 
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceSlashingDef, EvaluateParameters, TargetResistance); break;
	case ESG_DamageType::Piercing: 
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistancePiercingDef, EvaluateParameters, TargetResistance); break;
	case ESG_DamageType::Bludgeoning: 
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceBludgeoningDef, EvaluateParameters, TargetResistance); break;
	case ESG_DamageType::Fire: 
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceFireDef, EvaluateParameters, TargetResistance);
	}

	if (TargetResistance <= 0) TargetResistance = 1;

	if (bIsHitGuarded)
	{
		UE_LOG(DamageLog, Warning, TEXT("The hit is being guarded by the target"));
		
		float GuardProficiency = 1.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().GuardProficiencyDef, EvaluateParameters, GuardProficiency);
		TargetResistance *= GuardProficiency;
	}

	PenetrationMultiplier = (DamageSourceData->Penetration + DamageSourceData->Penetration * InstigatorVelocity / 100) / TargetResistance;

	return PenetrationMultiplier;
}




