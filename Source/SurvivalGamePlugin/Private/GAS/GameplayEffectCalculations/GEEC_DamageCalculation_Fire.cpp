// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculation_Fire.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameplayEffectTypes.h"

#include "Objects/GameplayEffectDataObjects/DamageSourceData.h"
#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"


UGEEC_DamageCalculation_Fire::UGEEC_DamageCalculation_Fire()
{
	RelevantAttributesToCapture.Add(DamageStatics().CurrentDexterityDef);

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

	float FireResistance = 1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceFireDef, EvaluateParameters, FireResistance);

	float FinalDefence = FireResistance - DamageSourceData->Penetration;
	float PenMultiplier = FMath::Floor(209 / (1 + FMath::Pow(2.71f, 0.014f * FinalDefence + 0.082f))) / 100;

	float FireDamage = FMath::Floor(DamageSourceData->BaseDamage * PenMultiplier);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -FireDamage));
	OutExecutionOutput.MarkConditionalGameplayEffectsToTrigger();

	float Dexterity = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentDexterityDef, EvaluateParameters, Dexterity);

	ApplyAdditionalEffects(TargetASC, DamageSourceData->AdditionalEffects, EffectContext, Dexterity);

	FGameplayCueParameters CueParams = FGameplayCueParameters();
	CueParams.Location = TargetActor->GetActorLocation();
	CueParams.EffectContext = EffectContext;
	CueParams.SourceObject = DamageSourceData;
	CueParams.RawMagnitude = FireDamage;

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

}
