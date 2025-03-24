// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_SpreadEffect.h"

#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemGlobals.h"

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
	USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MyTarget));
	if (IsValid(TargetASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(TargetASC) == false"));
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

	FVector MinLocalBounds = TargetRoot->GetLocalBounds().GetBox().Min;
	FVector MaxLocalBounds = TargetRoot->GetLocalBounds().GetBox().Max;

	FVector ActorBottom = MyTarget->GetActorLocation() + MyTarget->GetActorUpVector() * MinLocalBounds.Z;
	FVector ActorTop = MyTarget->GetActorLocation() + MyTarget->GetActorUpVector() * MaxLocalBounds.Z;

	FVector TraceCenter = (ActorTop + ActorBottom) / 2;
	FVector TraceExtent = TargetRoot->GetLocalBounds().GetBox().GetExtent() + ExtentFromTargetBounds;

	UE_LOG(LogTemp, Log, TEXT("UGC_SpreadEffect::OnExecute_Implementation: TraceCenter - %s, TraceExtend - %s"), *TraceCenter.ToString(), *TraceExtent.ToString());

	TArray<AActor*> ActorsToIgnore;
	if(bShouldIgnoreSelf) ActorsToIgnore.Add(MyTarget);

	TArray<FHitResult> HitResults;

	if (UKismetSystemLibrary::BoxTraceMulti(World, TraceCenter, TraceCenter, TraceExtent, MyTarget->GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2,
		true, ActorsToIgnore, bShouldDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, HitResults, true) == false)
	{
		return true;
	}

	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();

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

		USG_AbilitySystemComponent* HitActorASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor));
		if (IsValid(HitActorASC) == false)
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

			ContextHandle.AddSourceObject(GameplayEffectToSpread);

			FGameplayEffectSpecHandle SpecHandle = HitActorASC->MakeOutgoingSpec(GameplayEffectToSpread->DamageEffect, GameplayEffectToSpread->EffectLevel, ContextHandle);
			HitActorASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		else
		{
			UGameplayEffectData* ActiveGameplayEffectData = Cast<UGameplayEffectData>(ContextHandle.GetSourceObject());
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

			FGameplayEffectSpecHandle SpecHandle = HitActorASC->MakeOutgoingSpec(ActiveGameplayEffectData->DamageEffect, ActiveGameplayEffectData->EffectLevel, ContextHandle);
			HitActorASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		ActorsToIgnore.Add(HitActor);
	}

	return true;
}
