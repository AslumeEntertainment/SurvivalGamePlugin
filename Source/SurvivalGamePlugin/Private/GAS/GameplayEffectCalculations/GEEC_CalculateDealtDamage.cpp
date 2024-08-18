// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_CalculateDealtDamage.h"
#include "GAS/SG_AttributeSet.h"
#include "Characters/HumanCharacter.h"
#include "Characters/AnimalCharacter.h"
#include "Animation/SG_AnimInstance.h"


struct FSG_DamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageLeft);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageRight);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	FSG_DamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, DamageLeft, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, DamageRight, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, Health, Target, false);
	}
};

static const FSG_DamageStatics& DamageStatics()
{
	static FSG_DamageStatics DStatics;
	return DStatics;
}

UGEEC_CalculateDealtDamage::UGEEC_CalculateDealtDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageLeftDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageRightDef);
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void UGEEC_CalculateDealtDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	ASG_Character* SourceCharacter = Cast<ASG_Character>(SourceActor);

	FAggregatorEvaluateParameters EvaluationParameters;

	float DamageDone;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageRightDef, EvaluationParameters, DamageDone);

	if (Cast<AHumanCharacter>(SourceCharacter))
	{
		USG_AnimInstance* AnimInst = Cast<USG_AnimInstance>(SourceCharacter->GetMesh()->GetAnimInstance());
		if (!IsValid(AnimInst))
		{
			UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDealtDamage::Execute_Implementation IsValid(AnimInst) == false"));
			return;
		}
		if (AnimInst->bIsMirrored)
		{
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageLeftDef, EvaluationParameters, DamageDone);
		}
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -DamageDone));
}
