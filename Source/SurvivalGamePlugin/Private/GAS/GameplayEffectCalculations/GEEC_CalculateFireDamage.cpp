// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_CalculateFireDamage.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"


UGEEC_CalculateFireDamage::UGEEC_CalculateFireDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void UGEEC_CalculateFireDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	USG_AbilitySystemComponent* TargetAbilitySystemComponent = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetAbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateFireDamage::Execute_Implementation IsValid(TargetAbilitySystemComponent) == false"));
		return;
	}

	USG_AbilitySystemComponent* SourceAbilitySystemComponent = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetSourceAbilitySystemComponent());
	AActor* SourceActor = IsValid(SourceAbilitySystemComponent) ? SourceAbilitySystemComponent->GetOwner() : nullptr;

	if (false)
	{
		return;
	}

	if (IsValid(DefaultBurningEffect) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateFireDamage::Execute_Implementation IsValid(DefaultBurningEffect) == false"));
		return;
	}

	AActor* Instigator = ExecutionParams.GetOwningSpec().GetEffectContext().GetInstigator();
	TargetAbilitySystemComponent->AddEffectFromClass(DefaultBurningEffect, SourceActor, Instigator, Instigator);

}
