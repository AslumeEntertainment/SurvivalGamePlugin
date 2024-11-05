// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_OnStartedBurning.h"

#include "AbilitySystemInterface.h"

#include "Components/PassiveDamageDealingComponent.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/Abilities/SGGA_OnStartedBurning.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interfaces/HittableInterface.h"

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

	//TSubclassOf<UGameplayEffect> BurningEffect = ExecutionParams.GetOwningSpec().

	UPassiveDamageDealingComponent* TargetPassiveDamageDealer = TargetActor->GetComponentByClass<UPassiveDamageDealingComponent>();
	if (IsValid(TargetPassiveDamageDealer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::ActivateAbility IsValid(OwnerPassiveDamageDealer) == false"));
		return;
	}

	for (IHittableInterface* HittableActor : TargetPassiveDamageDealer->GetOverlappedActors())
	{
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(HittableActor);
		if (ASI == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::AttemptSpreadFire ASI == nullptr"));
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
