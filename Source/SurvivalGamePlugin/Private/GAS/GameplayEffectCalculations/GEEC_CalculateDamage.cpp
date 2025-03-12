// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_CalculateDamage.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/DamageDealingItem.h"
#include "Components/DamageDealingComponent.h"
#include "Headers/PhysicalPropertyEnums.h"
#include "Objects/DamageSourceData.h"

DEFINE_LOG_CATEGORY(DamageLog);

UGEEC_CalculateDamage::UGEEC_CalculateDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DexterityDef);

	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceSlashingDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistancePiercingDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceBludgeoningDef);
}

void UGEEC_CalculateDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	AActor* InstigatorActor = ExecutionParams.GetOwningSpec().GetEffectContext().GetInstigator();
	if (IsValid(InstigatorActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(InstigatorActor) == false"));
		return;
	}
	USG_AbilitySystemComponent* InstigatorAbilitySystemComponent = Cast<USG_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor));
	if (IsValid(InstigatorAbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(InstigatorAbilitySystemComponent) == false"));
		return;
	}
	USG_AbilitySystemComponent* TargetAbilitySystemComponent = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetAbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(TargetAbilitySystemComponent) == false"));
		return;
	}

	AActor* TargetActor = IsValid(TargetAbilitySystemComponent) ? TargetAbilitySystemComponent->GetOwner() : nullptr;

	const UDamageSourceData* DamageSourceData = Cast<UDamageSourceData>(ExecutionParams.GetOwningSpec().GetContext().GetSourceObject());
	if (IsValid(DamageSourceData) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(DamageSourceData) == false"));
		return;
	}

	AActor* EffectCauser = ExecutionParams.GetOwningSpec().GetEffectContext().GetEffectCauser();
	if (IsValid(EffectCauser) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(EffectCauser) == false"));
		return;
	}

	//-------------DAMAGE-FORMULA-(IN-DEVELOPMENT)-------------//

	EWeight EffectCauserWeight = EWeight::Normal;
	if (IsValid(Cast<ADamageDealingItem>(EffectCauser)))
	{
		EffectCauserWeight = Cast<ADamageDealingItem>(EffectCauser)->ItemData.ItemWeight;
	}

	float StrengthMultiplier = InstigatorAbilitySystemComponent->GetNumericAttribute(USG_CharacterAttributeSet::GetStrengthAttribute()) / (uint8)EffectCauserWeight;

	float TargetResistance = 1;
	switch (DamageSourceData->DamageType)
	{
		case ESG_DamageType::Slashing: TargetResistance = TargetAbilitySystemComponent->GetNumericAttribute(USG_AttributeSet::GetResistanceSlashingAttribute()); break;
		case ESG_DamageType::Piercing: TargetResistance = TargetAbilitySystemComponent->GetNumericAttribute(USG_AttributeSet::GetResistancePiercingAttribute()); break;
		case ESG_DamageType::Bludgeoning: TargetResistance = TargetAbilitySystemComponent->GetNumericAttribute(USG_AttributeSet::GetResistanceBludgeoningAttribute()); break;
		case ESG_DamageType::Fire: TargetResistance = TargetAbilitySystemComponent->GetNumericAttribute(USG_AttributeSet::GetResistanceFireAttribute());
	}
	if (TargetResistance <= 0) TargetResistance = 1;

	float InstigatorVelocity = InstigatorActor->GetVelocity().Length() / 100;
	float PenetrationMultiplier = (DamageSourceData->Penetration + DamageSourceData->Penetration * InstigatorVelocity / 100) / TargetResistance;
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

	int RandInt = FMath::RandRange(1, 100);
	RandInt += InstigatorAbilitySystemComponent->GetNumericAttribute(USG_CharacterAttributeSet::GetDexterityAttribute());

	for (TPair<TSubclassOf<UGameplayEffect>, float> AdditionalEffect : DamageSourceData->AdditionalEffects)
	{
		if (RandInt <= AdditionalEffect.Value)
		{
			TargetAbilitySystemComponent->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(AdditionalEffect.Key->GetDefaultObject()), DamageSourceData->EffectLevel, ExecutionParams.GetOwningSpec().GetEffectContext());
		}
	}

	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	EventData.Instigator = InstigatorActor;

	InstigatorAbilitySystemComponent->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Triggered"), &EventData);
	TargetAbilitySystemComponent->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Received"), &EventData);

}
