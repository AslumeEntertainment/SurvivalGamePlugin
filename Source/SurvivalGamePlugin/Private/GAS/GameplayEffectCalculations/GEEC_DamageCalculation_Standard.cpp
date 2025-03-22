// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_DamageCalculation_Standard.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/DamageDealingItem.h"
#include "Components/DamageDealingComponent.h"
#include "Headers/PhysicalPropertyEnums.h"
#include "Characters/InGameCharacter.h"
#include "Objects/DamageSourceData.h"

DEFINE_LOG_CATEGORY(DamageLog);

UGEEC_DamageCalculation_Standard::UGEEC_DamageCalculation_Standard()
{
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DexterityDef);

	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
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
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(TargetASC) == false"));
		return;
	}
	AActor* TargetActor = TargetASC->GetOwner();
	if (IsValid(TargetActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(TargetActor) == false"));
		return;
	}

	const UDamageSourceData* DamageSourceData = Cast<UDamageSourceData>(EffectContext.GetSourceObject());
	if (IsValid(DamageSourceData) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(DamageSourceData) == false"));
		return;
	}

	AActor* EffectCauser = EffectContext.GetEffectCauser();
	if (IsValid(EffectCauser) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(EffectCauser) == false"));
		return;
	}

	//-------------DAMAGE-FORMULA-(IN-DEVELOPMENT)-------------//

	FVector HitLocation = EffectContext.GetHitResult()->ImpactPoint;

	float StrengthMultiplier = GetStrengthMultiplier(InstigatorASC, EffectCauser);
	float InstigatorVelocity = IsValid(InstigatorActor) ? InstigatorActor->GetVelocity().Length() / 100 : 0;
	float PenetrationMultiplier = GetPenetrationMultiplier(DamageSourceData, InstigatorVelocity, TargetASC, HitLocation);

	float FinalDamage = FMath::Floor(DamageSourceData->BaseDamage * StrengthMultiplier * PenetrationMultiplier);
	
	if (bShouldPrintDebug)
	{
		UE_LOG(DamageLog, Log, TEXT("%s deals damage to %s using %s"), *InstigatorActor->GetName(), *TargetActor->GetName(), *EffectCauser->GetName());
		UE_LOG(DamageLog, Log, TEXT("Damage Type - %d (Slashing, Piercing, Bludgeoning, Fire)"), (uint8)DamageSourceData->DamageType);
		UE_LOG(DamageLog, Log, TEXT("StrengthMultiplier - %.2f"), StrengthMultiplier);
		UE_LOG(DamageLog, Log, TEXT("InstigatorVelocity - %.2f m/s"), InstigatorVelocity);
		UE_LOG(DamageLog, Log, TEXT("PenetrationMultiplier - %.2f"), PenetrationMultiplier);
		UE_LOG(DamageLog, Log, TEXT("FinalDamage - %.f"), FinalDamage);
	}

	//---------------------------------------------------------//

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -FinalDamage));

	FGameplayCueParameters CueParams = FGameplayCueParameters();

	CueParams.Location = EffectContext.GetHitResult()->GetActor() == TargetActor ? HitLocation : TargetActor->GetActorLocation();
	CueParams.EffectContext = EffectContext;
	CueParams.SourceObject = DamageSourceData;
	CueParams.RawMagnitude = FinalDamage;

	TriggerGameplayCues(TargetASC, CueParams);

	int RandInt = FMath::RandRange(1, 100);
	if (IsValid(InstigatorASC))
	{
		RandInt += InstigatorASC->GetNumericAttribute(USG_CharacterAttributeSet::GetDexterityAttribute());
	}
	
	FGameplayEffectContextHandle NewEffectContext = EffectContext;

	for (FAdditionalEffectDataStruct AdditionalEffect : DamageSourceData->AdditionalEffects)
	{
		if (RandInt <= AdditionalEffect.ChanceToApply)
		{
			if (IsValid(AdditionalEffect.AdditionalEffectData) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(AdditionalEffect.AdditionalEffectData) == false"));
				continue;
			}
			if (IsValid(AdditionalEffect.AdditionalEffectData->DamageEffect) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(AdditionalEffect.AdditionalEffectData->DamageEffect) == false"));
				continue;
			}

			NewEffectContext.AddSourceObject(AdditionalEffect.AdditionalEffectData);
			UE_LOG(LogTemp, Error, TEXT("AdditionalEffect.AdditionalEffectData - %s"), *NewEffectContext.GetSourceObject()->GetName());

			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(AdditionalEffect.AdditionalEffectData->DamageEffect, AdditionalEffect.AdditionalEffectData->EffectLevel, NewEffectContext);
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	EventData.Instigator = InstigatorActor;

	if (IsValid(InstigatorASC))
	{
		InstigatorASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Triggered"), &EventData);
	}
	TargetASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Received"), &EventData);

}

