// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_EnergyDrain.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"

struct FEnergyStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(EnergyDrainRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Energy);

	FEnergyStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_CharacterAttributeSet, Energy, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_CharacterAttributeSet, EnergyDrainRate, Target, false);
	}
};

static const FEnergyStatics& EnergyStatics()
{
	static FEnergyStatics EStatics;
	return EStatics;
}

UGEEC_EnergyDrain::UGEEC_EnergyDrain()
{
	RelevantAttributesToCapture.Add(EnergyStatics().EnergyDef);
	RelevantAttributesToCapture.Add(EnergyStatics().EnergyDrainRateDef);
}

void UGEEC_EnergyDrain::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_EnergyDrain::Execute_Implementation IsValid(TargetASC) == false"));
		return;
	}

	AInGameCharacter* Target = Cast<AInGameCharacter>(TargetASC->GetOwner());
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_EnergyDrain::Execute_Implementation IsValid(Target) == false"));
		return;
	}

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

	float EnergyDrainRate = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EnergyStatics().EnergyDrainRateDef, EvaluateParameters, EnergyDrainRate);

	EnergyDrainRate *= (Target->GetCharacterMovement()->IsFalling() || Target->GetVelocity().Length() == 0) ? 0.f : 1.f;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(EnergyStatics().EnergyProperty, EGameplayModOp::Additive, -EnergyDrainRate));

}
