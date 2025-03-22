// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_SpreadEffect.h"

#include "Kismet/KismetSystemLibrary.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Objects/GameplayEffectData.h"


bool UGC_SpreadEffect::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (IsValid(MyTarget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(MyTarget) == false"));
		return false;
	}
	USceneComponent* TargetRoot = MyTarget->GetRootComponent();
	if (IsValid(TargetRoot) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(TargetActor) == false"));
		return false;
	}
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(World) == false"));
		return false;
	}

	FVector TraceCenter = MyTarget->GetActorLocation() + TargetRoot->GetLocalBounds().GetBox().GetCenter(); 
	FVector TraceExtent = TargetRoot->GetLocalBounds().GetBox().GetExtent() + ExtentFromTargetBounds;

	UE_LOG(LogTemp, Error, TEXT("TraceCenter - %s, TraceExtend - %s"), *TraceCenter.ToString(), *TraceExtent.ToString());

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MyTarget);

	TArray<FHitResult> HitResults;

	if (UKismetSystemLibrary::BoxTraceMulti(World, TraceCenter, TraceCenter, TraceExtent, MyTarget->GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2,
		true, ActorsToIgnore, bShouldDrawDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, HitResults, true) == false)
	{
		return true;
	}

	FGameplayEffectContextHandle EffectContext = Parameters.EffectContext;

	for (FHitResult HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) == false)
		{
			continue;
		}
		if (ActorsToIgnore.Contains(HitActor))
		{
			continue;
		}

		USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor));
		if (IsValid(TargetASC) == false)
		{
			continue;
		}

		if (IsValid(GameplayEffectToSpread))
		{
			if (IsValid(GameplayEffectToSpread->DamageEffect) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(GameplayEffectToSpread->DamageEffect) == false"));
				return false;
			}

			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectToSpread->DamageEffect, GameplayEffectToSpread->EffectLevel, EffectContext);
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		else
		{
			UGameplayEffectData* ActiveGameplayEffectData = Cast<UGameplayEffectData>(EffectContext.GetSourceObject());
			if (IsValid(ActiveGameplayEffectData) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(ActiveGameplayEffectData) == false"));
				return false;
			}
			if (IsValid(ActiveGameplayEffectData->DamageEffect) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(ActiveGameplayEffectData->DamageEffect) == false"));
				return false;
			}

			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(ActiveGameplayEffectData->DamageEffect, ActiveGameplayEffectData->EffectLevel, EffectContext);
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		ActorsToIgnore.Add(HitActor);
	}

	return true;
}
