// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_OnStartedBurning.h"

#include "AbilitySystemInterface.h"

#include "Components/HittableComponent.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"


void UGEEC_OnStartedBurning::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	USG_AbilitySystemComponent* TargetAbilitySystemComponent = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (IsValid(TargetAbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_OnStartedBurning::Execute_Implementation IsValid(TargetAbilitySystemComponent) == false"));
		return;
	}

	AActor* TargetActor = IsValid(TargetAbilitySystemComponent) ? TargetAbilitySystemComponent->GetOwner() : nullptr;

	if (IsValid(TargetActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_OnStartedBurning::Execute_Implementation IsValid(TargetActor) == false"));
		return;
	}

	UHittableComponent* TargetHittableComp = TargetActor->GetComponentByClass<UHittableComponent>();
	if (IsValid(TargetHittableComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::ActivateAbility IsValid(TargetHittableComp) == false"));
		return;
	}

	TArray<AActor*> OverlappingActors;
	TargetHittableComp->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == TargetActor)
		{
			continue;
		}
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor);
		if (ASI == nullptr)
		{
			continue;
		}

		USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(ASI->GetAbilitySystemComponent());
		if (IsValid(ASC) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::AttemptSpreadFire IsValid(ASC) == false"));
			continue;
		}

		int Amount = FMath::RandRange(1, 100);
		if (Amount <= ChanceToSpread)
		{
			ASC->AddEffectFromClass(BurningEffect, TargetActor);
		}
	}
}
