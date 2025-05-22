// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_ImpactDamage.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Characters/InGameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


UGEEC_ImpactDamage::UGEEC_ImpactDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().CurrentHealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().CurrentBludgeoningDefenceDef);
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
	/*const FHitResult* HitResult = EffectContext.GetHitResult();
	if (HitResult == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::Execute_Implementation HitResult == nullptr"));
		return;
	}*/

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("=================%s: Impact triggered================"), *TargetActor->GetName());
	}

	//float InternalImpactDamage = CalculateInternalImpactDamage(TargetActor, HitResult);
	//float ExternalImpactDamage = CalculateExternalImpactDamage(TargetActor, HitResult);

	float FinalDamage = CalculateSharedImpactDamage(TargetActor, EffectContext.GetInstigator());// InternalImpactDamage > ExternalImpactDamage ? InternalImpactDamage : ExternalImpactDamage;

	float TargetResistanceBludgeoning = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentBludgeoningDefenceDef, EvaluateParameters, TargetResistanceBludgeoning);

	if (bShouldPrintDebug) UE_LOG(LogTemp, Warning, TEXT("Impact Damage before defence is applied - %.f"), FinalDamage);

	FinalDamage = FMath::Floor(FinalDamage * FMath::Floor(209 / (1 + FMath::Pow(2.71f, 0.014f * TargetResistanceBludgeoning + 0.082f))) / 100);

	if (bShouldPrintDebug) UE_LOG(LogTemp, Warning, TEXT("Impact Damage after defence is applied - %.f"), FinalDamage);

	if (FinalDamage <= 0.f)
	{
		if (bShouldPrintDebug) UE_LOG(LogTemp, Warning, TEXT("Impact wasn't strong enough to deal damage"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Final impact damage - %.2f"), FinalDamage);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().CurrentHealthProperty, EGameplayModOp::Additive, -FinalDamage));
	OutExecutionOutput.MarkConditionalGameplayEffectsToTrigger();


	FGameplayCueParameters CueParams = FGameplayCueParameters();
	CueParams.Location = FVector(TargetActor->GetActorLocation());
	CueParams.RawMagnitude = FinalDamage;

	TriggerGameplayCues(TargetASC, CueParams);

	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	EventData.ContextHandle = EffectContext;

	AcknowledgeHit(nullptr, TargetASC, EventData);

	return;
}

float UGEEC_ImpactDamage::CalculateSharedImpactDamage(AActor* ActorBeeingHit, AActor* HittingActor) const
{
	float Speed = (ActorBeeingHit->GetVelocity() - HittingActor->GetVelocity()).Size();
	float HitActorMass = 1;
	AInGameCharacter* Character = Cast<AInGameCharacter>(ActorBeeingHit);

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor being hit velocity - %.2f"), ActorBeeingHit->GetVelocity().Length());
		UE_LOG(LogTemp, Warning, TEXT("Hitting Actor velocity - %.2f"), HittingActor->GetVelocity().Length());
	}

	if (IsValid(Character))
	{
		HitActorMass = Character->GetCharacterMovement()->Mass;
	}
	else
	{
		UMeshComponent* ActorBeeingHitMeshComp = ActorBeeingHit->GetComponentByClass<UMeshComponent>();
		if (IsValid(ActorBeeingHitMeshComp) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::CalculateExternalImpactDamage IsValid(HitActorMeshComp) == false"));
			return 0;
		}
		HitActorMass = ActorBeeingHitMeshComp->GetMass();
	}


	bool isFalling = (ActorBeeingHit->GetVelocity().GetSafeNormal().Z < ZVelocityFallAngle);

	float Damage = ((HitActorMass * (Speed / 100) * (Speed / 100)) / 1000) * (isFalling ? FallDamageMultiplier : HorizontalDamageMultiplier);

	return (Damage < MininumDamage) ? 0 : Damage;
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
			25, FLinearColor::Red, 10.f, 1.f);
		UKismetSystemLibrary::DrawDebugArrow(TargetActor->GetWorld(), TargetActor->GetActorLocation(), TargetActor->GetActorLocation() + NormalizedVelocity * 100,
			25, FLinearColor::Green, 10.f, 1.f);

		UE_LOG(LogTemp, Warning, TEXT("Impact Angle - %.2f deg"), FMath::Abs(CollisionAngle));
	}

	if (FMath::Abs(CollisionAngle) < InternalImpactDamageThresholdAngle)
	{
		return Damage; 
	}
	
	float VerticalCoeficient = FMath::Clamp(-1 * NormalizedVelocity.Z, 0.f, 1.f);

	Damage = (VelocityLength * 0.065 - 90) * 1.2 + (40 * VerticalCoeficient);
	//Damage = (VelocityLength * 0.05 - 50) * (VerticalCoeficient + 1);
	//Damage = (NormalizedVelocity.Z <= -0.45) ? FMath::Abs(Velocity.Z) * 0.08 - 60 : VelocityLength * 0.057 - 71;

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Velocity - %.2f"), VelocityLength);
		UE_LOG(LogTemp, Warning, TEXT("Vertical Coeficient - %.2f"), VerticalCoeficient);
		UE_LOG(LogTemp, Warning, TEXT("Internal Impact Damage - %.2f"), Damage);
	}

	return Damage;
}

float UGEEC_ImpactDamage::CalculateExternalImpactDamage(AActor* TargetActor, const FHitResult* HitResult) const
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

	if (TargetActor->GetVelocity().Length() > 0.f)
	{
		float CollisionAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(TargetActor->GetVelocity(), HitActor->GetVelocity()));
		if (bShouldPrintDebug) UE_LOG(LogTemp, Warning, TEXT("External Impact Angle - %.2f deg"), FMath::Abs(CollisionAngle));
		
		if (FMath::Abs(CollisionAngle) < ExternalImpactDamageThresholdAngle)
		{
			return Damage;
		}
	}
	
	UMeshComponent* HitActorMeshComp = HitActor->GetComponentByClass<UMeshComponent>();
	if (IsValid(HitActorMeshComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ImpactDamage::CalculateExternalImpactDamage IsValid(HitActorMeshComp) == false"));
		return Damage;
	}

	float HitActorMass = HitActorMeshComp->GetMass();

	Damage = Velocity/100 * HitActorMass/10;

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("==========ExternalImpactDamageCalculation=========="));
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor - %s"), *HitActor->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor Velocity - %.2f"), HitActor->GetVelocity().Length());
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor Mass - %.2f"), HitActorMass);
		UE_LOG(LogTemp, Warning, TEXT("External Impact Damage - %.2f"), Damage);
	}

	return Damage;
}
