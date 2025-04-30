// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_ImpactDamage.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"


UGEEC_ImpactDamage::UGEEC_ImpactDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceBludgeoningDef);
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

	FGameplayEffectContextHandle EffectContext = ExecutionParams.GetOwningSpec().GetEffectContext();
	const FHitResult* HitResult = EffectContext.GetHitResult();
	if (HitResult == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::Execute_Implementation HitResult == nullptr"));
		return;
	}

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("=================%s: Impact triggered================"), *TargetActor->GetName());
	}

	float InternalImpactDamage = CalculateInternalImpactDamage(TargetActor, HitResult);
	float ExternalImpactDamage = CalculateExternalImpactDamage(HitResult);
	float TrueDamage = InternalImpactDamage > ExternalImpactDamage ? InternalImpactDamage : ExternalImpactDamage;

	float TargetResistanceBludgeoning = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ResistanceBludgeoningDef, EvaluateParameters, TargetResistanceBludgeoning);

	TrueDamage = FMath::Floor(TrueDamage - TargetResistanceBludgeoning);

	if (TrueDamage <= 0.f)
	{
		if (bShouldPrintDebug) UE_LOG(LogTemp, Warning, TEXT("Impact wasn't strong enough to deal damage"));
		return;
	}
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -TrueDamage));
	OutExecutionOutput.MarkConditionalGameplayEffectsToTrigger();

	FGameplayCueParameters CueParams = FGameplayCueParameters();
	CueParams.Location = FVector(HitResult->ImpactPoint);
	CueParams.RawMagnitude = TrueDamage;

	TriggerGameplayCues(TargetASC, CueParams);

	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	EventData.ContextHandle = EffectContext;

	AcknowledgeHit(nullptr, TargetASC, EventData);

	return;
}

float UGEEC_ImpactDamage::CalculateInternalImpactDamage(AActor* TargetActor, const FHitResult* HitResult) const
{
	float Damage = 0.f;

	FVector Velocity = TargetActor->GetVelocity();
	float VelocityLength = Velocity.Length();

	if (VelocityLength <= 0.f)
	{
		return Damage;
	}

	FVector NormalizedVelocity = UKismetMathLibrary::Normal(Velocity);
	float CollisionAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(HitResult->ImpactNormal, NormalizedVelocity));
	
	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("=========InternalImpactDamageCalculation=========="));

		UKismetSystemLibrary::DrawDebugArrow(TargetActor->GetWorld(), HitResult->ImpactPoint, HitResult->ImpactPoint + HitResult->ImpactNormal * 100,
			25, FLinearColor::Red, 20.f, 1.f);
		UKismetSystemLibrary::DrawDebugArrow(TargetActor->GetWorld(), TargetActor->GetActorLocation(), TargetActor->GetActorLocation() + NormalizedVelocity * 100,
			25, FLinearColor::Green, 20.f, 1.f);

		UE_LOG(LogTemp, Warning, TEXT("Impact Angle - %.2f deg"), FMath::Abs(CollisionAngle));
	}

	if (FMath::Abs(CollisionAngle) < ImpactDamageThresholdAngle)
	{
		return Damage; 
	}
	
	float VerticalCoeficient = FMath::Clamp(-1 * Velocity.Z, 0.f, 1.f);

	Damage = VerticalCoeficient * 15 + VelocityLength * 0.05 - 50;
	//Damage = (NormalizedVelocity.Z <= -0.45) ? FMath::Abs(Velocity.Z) * 0.08 - 60 : VelocityLength * 0.057 - 71;

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Velocity - %.2f"), VelocityLength);
		UE_LOG(LogTemp, Warning, TEXT("VerticalCoeficient - %.2f"), VerticalCoeficient);
		UE_LOG(LogTemp, Warning, TEXT("Internal impact damage - %.2f"), Damage);
	}

	return Damage;
}

float UGEEC_ImpactDamage::CalculateExternalImpactDamage(const FHitResult* HitResult) const
{
	float Damage = 0.f;

	AActor* HitActor = HitResult->GetActor();
	if (IsValid(HitActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::CalculateExternalImpactDamage IsValid(HitActor) == false"));
		return Damage;
	}

	float Velocity = HitActor->GetVelocity().Length();
	if (Velocity <= 0.f)
	{
		return Damage;
	}

	UMeshComponent* HitActorMeshComp = HitActor->GetComponentByClass<UMeshComponent>();
	if (IsValid(HitActorMeshComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::CalculateExternalImpactDamage IsValid(HitActorMeshComp) == false"));
		return Damage;
	}

	float HitActorMass = HitActorMeshComp->IsSimulatingPhysics() ? HitActorMeshComp->GetMass() : 1.f;

	Damage = Velocity/100 * HitActorMass;
	//Damage = Velocity * 0.057 - 71;

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("==========ExternalImpactDamageCalculation=========="));
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor - %s"), *HitActor->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor velocity - %.2f"), HitActor->GetVelocity().Length());
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor mass - %.2f"), HitActorMass);
		UE_LOG(LogTemp, Warning, TEXT("External impact damage - %.2f"), Damage);
	}

	return Damage;
}
