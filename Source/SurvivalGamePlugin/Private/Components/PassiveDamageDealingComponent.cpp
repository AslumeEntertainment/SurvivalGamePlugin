// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PassiveDamageDealingComponent.h"

#include "Interfaces/HittableInterface.h"


UPassiveDamageDealingComponent::UPassiveDamageDealingComponent()
{
}

void UPassiveDamageDealingComponent::BeginPlay()
{
	Super::BeginPlay();
}

//void UPassiveDamageDealingComponent::ApplyInstantGameplayEffectSpecHandle(FGameplayEffectSpecHandle SpecHandle, bool bShouldApplyOnOverlap)
//{
//	TArray<FGameplayEffectSpecHandle> SpecHandlesToApply;
//	SpecHandlesToApply.Add(SpecHandle);
//
//	TArray<AActor*> OverlappingActors;
//	TargetPassiveDamageDealer->GetOverlappingActors(OverlappingActors);
//
//	for (IHittableInterface* OverlappedActor : OverlappedActors)
//	{
//		OverlappedActor->OnHit(SpecHandlesToApply);
//	}
//
//	for (AActor* Actor : OverlappingActors)
//	{
//		if (Actor == TargetActor)
//		{
//			continue;
//		}
//		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor);
//		if (ASI == nullptr)
//		{
//			continue;
//		}
//
//		USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(ASI->GetAbilitySystemComponent());
//		if (IsValid(ASC) == false)
//		{
//			UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::AttemptSpreadFire IsValid(ASC) == false"));
//			continue;
//		}
//
//		int Amount = FMath::RandRange(1, 100);
//		if (Amount <= ChanceToSpread)
//		{
//			ASC->AddEffectFromClass(BurningEffect, TargetActor);
//		}
//	}
//}