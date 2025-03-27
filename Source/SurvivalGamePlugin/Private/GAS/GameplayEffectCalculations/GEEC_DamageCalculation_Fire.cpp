// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculation_Fire.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameplayEffectTypes.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Objects/DamageSourceData.h"


UGEEC_DamageCalculation_Fire::UGEEC_DamageCalculation_Fire()
{
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceFireDef);
}

void UGEEC_DamageCalculation_Fire::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FGameplayEffectContextHandle EffectContext = ExecutionParams.GetOwningSpec().GetEffectContext();

	AActor* InstigatorActor = EffectContext.GetInstigator();
	USG_AbilitySystemComponent* InstigatorASC = IsValid(InstigatorActor) ? Cast<USG_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor)) : nullptr;

	USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Fire::Execute_Implementation IsValid(TargetASC) == false"));
		return;
	}
	AActor* TargetActor = TargetASC->GetOwner();
	if (IsValid(TargetActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Fire::Execute_Implementation IsValid(TargetActor) == false"));
		return;
	}

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

	const UDamageSourceData* DamageSourceData = Cast<UDamageSourceData>(EffectContext.GetSourceObject());
	if (IsValid(DamageSourceData) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_DamageCalculation_Fire::Execute_Implementation IsValid(DamageSourceData) == false"));
		return;
	}

	if (IsTargetImmuneToDamageType(TargetASC, DamageSourceData->DamageType))
	{
		return;
	}

	float FireResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceFireDef, EvaluateParameters, FireResistance);

	float FireDamage = FMath::Floor(DamageSourceData->BaseDamage / (FireResistance <= 0.f ? 1 : FireResistance));

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -FireDamage));
	OutExecutionOutput.MarkConditionalGameplayEffectsToTrigger();

	float Dexterity = IsValid(InstigatorASC) ? InstigatorASC->GetNumericAttribute(USG_CharacterAttributeSet::GetDexterityAttribute()) : 0.f;
	ApplyAdditionalEffects(TargetASC, DamageSourceData->AdditionalEffects, EffectContext, Dexterity);

	FGameplayCueParameters CueParams = FGameplayCueParameters();
	CueParams.Location = TargetActor->GetActorLocation();
	CueParams.EffectContext = EffectContext;
	CueParams.SourceObject = DamageSourceData;
	CueParams.RawMagnitude = FireDamage;

	TriggerGameplayCues(TargetASC, CueParams);

	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	EventData.Instigator = InstigatorActor;

	AcknowledgeHit(InstigatorASC, TargetASC, EventData);

	return;
}
