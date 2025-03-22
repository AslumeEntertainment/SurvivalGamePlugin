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
	USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::Execute_Implementation IsValid(TargetASC) == false"));
		return;
	}
	AActor* TargetActor = TargetASC->GetAvatarActor();
	if (IsValid(TargetActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::Execute_Implementation IsValid(TargetActor) == false"));
		return;
	}

	float Damage = 0;
	FVector Velocity = TargetActor->GetVelocity();
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

	Damage = FMath::Clamp<float>(Damage, 0, TargetASC->GetNumericAttribute(USG_AttributeSet::GetMaxHealthAttribute()));
	Damage = FMath::CeilToFloat(Damage);
	Damage -= TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistanceBludgeoningAttribute());

	if (Damage <= 0.f)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Velocity on %s - %f"), *DamageContextString, Velocity.Length());
	UE_LOG(LogTemp, Warning, TEXT("%s damage - %f"), *DamageContextString, Damage);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -Damage));

	FGameplayCueParameters CueParams = FGameplayCueParameters();

	CueParams.Location = TargetActor->GetActorLocation();
	CueParams.RawMagnitude = Damage;

	TriggerGameplayCues(TargetASC, CueParams);

	FGameplayEventData EventData;
	EventData.Target = TargetActor;

	TargetASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Received"), &EventData);
}
