// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_CalculateFireDamage.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_GameplayEffectContext.h"
#include "GAS/SG_AbilitySystemGlobals.h"


UGEEC_CalculateFireDamage::UGEEC_CalculateFireDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void UGEEC_CalculateFireDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!IsValid(TargetAbilitySystemComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateFireDamage::Execute_Implementation IsValid(TargetAbilitySystemComponent) == false"));
		return;
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	float BaseDamage = Spec.GetSetByCallerMagnitude(SetByCallerMagnitudeDataTag, false, -1.0f);
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, BaseDamage));

	if (false)
	{
		return;
	}

	if (!IsValid(DefaultBurningEffect.DamageEffectClass))
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateFireDamage::Execute_Implementation IsValid(NewGameplayEffect.DamageEffectClass) == false"));
		return;
	}

	FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(USG_AbilitySystemGlobals::Get().AllocGameplayEffectContext());
	FSG_GameplayEffectContext* Context = static_cast<FSG_GameplayEffectContext*>(EffectContextHandle.Get());
	Context->SetDamageType(DefaultBurningEffect.DamageType);
	EffectContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle BurningSpecHandle = UAbilitySystemBlueprintLibrary::MakeSpecHandle(DefaultBurningEffect.DamageEffectClass.GetDefaultObject(), nullptr, nullptr);
	FGameplayEffectSpec* BurningSpec = BurningSpecHandle.Data.Get();

	BurningSpec->SetContext(EffectContextHandle);
	BurningSpec->SetSetByCallerMagnitude(SetByCallerMagnitudeEffectDurationTag, DefaultBurningEffect.DamageEffectDuration);
	BurningSpec->Period = DefaultBurningEffect.DamageEffectPeriod;
	BurningSpec->SetSetByCallerMagnitude(DefaultBurningEffect.SetByCallerMagnitudeDataTag, -DefaultBurningEffect.DamageValue);

	TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*BurningSpec);

}
