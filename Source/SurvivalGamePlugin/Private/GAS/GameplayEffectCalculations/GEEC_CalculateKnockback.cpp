// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_CalculateKnockback.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Objects/DamageSourceData.h"
#include "Characters/SG_Character.h"

void UGEEC_CalculateKnockback::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UDamageSourceData* DamageSourceData = Cast<UDamageSourceData>(ExecutionParams.GetOwningSpec().GetContext().GetSourceObject());
	if (IsValid(DamageSourceData) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateKnockback::Execute_Implementation IsValid(DamageSourceData) == false"));
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
	
	FVector KnockbackForce =  InstigatorActor->GetActorUpVector() * DamageSourceData->BaseDamage;

	ASG_Character* TargetCharacter = Cast<ASG_Character>(TargetActor);
	if (IsValid(TargetCharacter))
	{
		TargetCharacter->LaunchCharacter(KnockbackForce, false, false);
	}
	else
	{
		UMeshComponent* TargetActorMesh = TargetActor->GetComponentByClass<UMeshComponent>();
		if (IsValid(TargetActorMesh))
		{
			TargetActorMesh->AddImpulse(KnockbackForce);
		}
	}
}
