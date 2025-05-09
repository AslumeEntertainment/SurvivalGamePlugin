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
	RelevantAttributesToCapture.Add(DamageStatics().CurrentStrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurrentDexterityDef);

	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurrentGuardProficiencyDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurrentSlashingDefenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurrentPiercingDefenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurrentBludgeoningDefenceDef);
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
		float PenetrationMultiplier = GetPenetrationMultiplier(ExecutionParams, EvaluateParameters, DamageSourceData, 
			EffectCauser, InstigatorVelocity, HittableTarget->IsHitGuarded(HitLocation));

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
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentDexterityDef, EvaluateParameters, Dexterity);

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
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentStrengthDef, EvaluateParameters, Strength);
	if (Strength <= 0.f)
	{
		return StrengthMultiplier;
	}

	if (IsValid(EffectCauser) == false)
	{
		return StrengthMultiplier;
	}

	EWeight EffectCauserWeight = EWeight::Normal;

	const ADamageDealingItem* Weapon = Cast<ADamageDealingItem>(EffectCauser);
	if (IsValid(Weapon))
	{
		EffectCauserWeight = Weapon->ItemData.ItemWeight;
	}

	for (EWeight Weight : TEnumRange<EWeight>())
	{
		if (UEnum::GetValueAsString(Weight).Compare("None") == 0) continue;

		if ((uint8)Weight <= (uint8)EffectCauserWeight) continue;

		if ((uint8)Weight > Strength) break;

		StrengthMultiplier += 0.25f;
	}

	return StrengthMultiplier;
}

float UGEEC_DamageCalculation_Standard::GetPenetrationMultiplier(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters EvaluateParameters, const UDamageSourceData* DamageSourceData, AActor* EffectCauser, float InstigatorVelocity, bool bIsHitGuarded) const
{
	float PenetrationMultiplier = 1.f;

	float TargetResistance = 0.f;

	switch (DamageSourceData->DamageType)
	{
	case ESG_DamageType::Slashing: 
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentSlashingDefenceDef, EvaluateParameters, TargetResistance); break;
	case ESG_DamageType::Piercing:
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentPiercingDefenceDef, EvaluateParameters, TargetResistance); break;
	case ESG_DamageType::Bludgeoning:
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentBludgeoningDefenceDef, EvaluateParameters, TargetResistance); break;
	case ESG_DamageType::Fire:
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentFireDefenceDef, EvaluateParameters, TargetResistance);
	}

	if (bIsHitGuarded)
	{
		UE_LOG(DamageLog, Warning, TEXT("The hit is being guarded by the target"));
		
		float GuardProficiency = 1.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentGuardProficiencyDef, EvaluateParameters, GuardProficiency);
		TargetResistance *= GuardProficiency;
	}

	float FinalPenetration = DamageSourceData->Penetration + DamageSourceData->Penetration * InstigatorVelocity / 100;
	float FinalDefence = TargetResistance - FinalPenetration;

	PenetrationMultiplier = FMath::Floor(209 / (1 + FMath::Pow(2.71f, 0.014f * FinalDefence + 0.082f))) / 100;

	ADamageDealingItem* Weapon = Cast<ADamageDealingItem>(EffectCauser);
	if (IsValid(Weapon))
	{
		float DurabilityLossMultiplier = FMath::Floor(-209 / (1 + FMath::Pow(2, 0.012f * FinalDefence + 0.1244f)) + 200) / 100;
		Weapon->OnLandedAHit.Broadcast(DurabilityLossMultiplier);
	}

	return PenetrationMultiplier;
}




