// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_CalculateDamage.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/DamageEffect.h"

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

	const UDamageEffect* DamageEffect = Cast<UDamageEffect>(ExecutionParams.GetOwningSpec().Def);
	if (IsValid(DamageEffect) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(DamageEffect) == false"));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation Instigator: %s; Target: %s"), *InstigatorActor->GetName(), *TargetActor->GetName());
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -DamageEffect->BaseDamage));
}
