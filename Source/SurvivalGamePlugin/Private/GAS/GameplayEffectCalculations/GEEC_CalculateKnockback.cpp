// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_CalculateKnockback.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Objects/KnockbackEffectData.h"
#include "Characters/SG_Character.h"
#include "Interfaces/Hittable.h"


void UGEEC_CalculateKnockback::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UKnockbackEffectData* KnockbackEffectData = Cast<UKnockbackEffectData>(ExecutionParams.GetOwningSpec().GetContext().GetSourceObject());
	if (IsValid(KnockbackEffectData) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateKnockback::Execute_Implementation IsValid(KnockbackEffectData) == false"));
		return;
	}
	AActor* InstigatorActor = ExecutionParams.GetOwningSpec().GetEffectContext().GetInstigator();
	if (IsValid(InstigatorActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateKnockback::Execute_Implementation IsValid(InstigatorActor) == false"));
		return;
	}
	USG_AbilitySystemComponent* TargetAbilitySystemComponent = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetAbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateKnockback::Execute_Implementation IsValid(TargetAbilitySystemComponent) == false"));
		return;
	}
	AActor* TargetActor = TargetAbilitySystemComponent->GetOwner();
	if (IsValid(TargetActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateKnockback::Execute_Implementation IsValid(TargetActor) == false"));
		return;
	}
	IHittable* HittableTarget = Cast<IHittable>(TargetActor);
	if (HittableTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateKnockback::Execute_Implementation HittableTarget == nullptr"));
		return;
	}
	
	FVector LaunchDirection;
	if (KnockbackEffectData->bUseHitDirection)
	{
		LaunchDirection = ExecutionParams.GetOwningSpec().GetEffectContext().GetHitResult()->ImpactNormal * -1;
	}
	else
	{
		LaunchDirection = InstigatorActor->GetActorUpVector();
	}
	UE_LOG(LogTemp, Log, TEXT("%s was launched in this direction - %s"), *TargetActor->GetName(), *LaunchDirection.ToString());

	FVector KnockbackForce = LaunchDirection * KnockbackEffectData->KnockbackPower * KnockbackEffectData->EffectLevel;

	if (HittableTarget->IsGuarding())
	{
		KnockbackForce /= TargetAbilitySystemComponent->GetNumericAttribute(USG_AttributeSet::GetGuardProficiencyAttribute());
	}

	ASG_Character* TargetCharacter = Cast<ASG_Character>(TargetActor);
	if (IsValid(TargetCharacter))
	{
		TargetCharacter->LaunchCharacter(KnockbackForce, false, false);
	}
	else
	{
		UPrimitiveComponent* TargetRoot = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent());
		if (IsValid(TargetRoot))
		{
			TargetRoot->AddImpulse(KnockbackForce);
		}
	}
}
