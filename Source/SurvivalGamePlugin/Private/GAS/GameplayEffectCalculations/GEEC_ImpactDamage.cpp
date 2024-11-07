// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_ImpactDamage.h"

#include "Kismet/KismetMathLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"


UGEEC_ImpactDamage::UGEEC_ImpactDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void UGEEC_ImpactDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	USG_AbilitySystemComponent* TargetAbilitySystemComponent = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetAbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::Execute_Implementation IsValid(TargetAbilitySystemComponent) == false"));
		return;
	}
	AActor* Target = TargetAbilitySystemComponent->GetAvatarActor();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::Execute_Implementation IsValid(Target) == false"));
		return;
	}

	float Damage = 0;
	FVector Velocity = Target->GetVelocity();
	FVector NormalVelocity = UKismetMathLibrary::Normal(Velocity);
	FString DamageContextString;

	if (NormalVelocity.Z <= -0.45)
	{
		Damage = FMath::Abs(Velocity.Z) * 0.08 - 60;
		DamageContextString = "Fall";
	}
	else
	{
		Damage = Velocity.Length() * 0.057 - 71;
		DamageContextString = "Impact";
	}

	// Damage *= Actor.Vulnerability
	Damage = FMath::Clamp<float>(Damage, 0, TargetAbilitySystemComponent->GetNumericAttribute(USG_AttributeSet::GetMaxHealthAttribute()));
	Damage = FMath::CeilToFloat(Damage);

	if (Damage > 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Velocity on %s - %f"), *DamageContextString, Velocity.Length()));
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("%s damage - %f"), *DamageContextString, Damage));

		TargetAbilitySystemComponent->AddEffectFromClass(InvincibilityEffectClass, nullptr);
		TargetAbilitySystemComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.RecieveDamage"));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -Damage));
	}

}
