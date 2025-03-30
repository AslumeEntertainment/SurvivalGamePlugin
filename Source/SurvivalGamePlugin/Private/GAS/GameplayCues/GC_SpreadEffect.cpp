// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_SpreadEffect.h"

#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemGlobals.h"

#include "Objects/GameplayEffectDataObjects/GameplayEffectData.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"


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

	//UE_LOG(LogTemp, Log, TEXT("UGC_SpreadEffect::OnExecute_Implementation: TraceCenter - %s, TraceExtend - %s"), *TraceCenter.ToString(), *TraceExtent.ToString());

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

		FGameplayEffectSpecHandle SpecHandle;

		const UGameplayEffectData* ActiveGameplayEffectData = Cast<UGameplayEffectData>(Parameters.GetSourceObject());

		if (IsValid(GameplayEffectToSpread))
		{
			if (IsValid(GameplayEffectToSpread->EffectClass) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(GameplayEffectToSpread->EffectClass) == false"));
				return false;
			}

			ContextHandle.AddSourceObject(GameplayEffectToSpread);
			SpecHandle = HitActorASC->MakeOutgoingSpec(GameplayEffectToSpread->EffectClass, GameplayEffectToSpread->EffectLevel, ContextHandle);
		}
		else
		{
			if (IsValid(ActiveGameplayEffectData))
			{
				UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(ActiveGameplayEffectData) == false"));
				return false;
			}
			if (IsValid(ActiveGameplayEffectData->EffectClass) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UGC_SpreadEffect::OnExecute_Implementation IsValid(ActiveGameplayEffectData->EffectClass) == false"));
				return false;
			}

			ContextHandle.AddSourceObject(ActiveGameplayEffectData);
			SpecHandle = HitActorASC->MakeOutgoingSpec(ActiveGameplayEffectData->EffectClass, ActiveGameplayEffectData->EffectLevel, ContextHandle);
		}

		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (Spec == nullptr)
		{
			continue;
		}

		HitActorASC->ApplyGameplayEffectSpecToSelf(*Spec);
		UE_LOG(LogTemp, Warning, TEXT("UGC_SpreadEffect::OnExecute_Implementation Spreading effect from %s to %s"), *MyTarget->GetName(), *HitActor->GetName());
		if (TargetASC->GetNumericAttribute(USG_AttributeSet::GetHealthAttribute()) <= 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s has 0 health, this cue should not exist!!!"), *MyTarget->GetName());
		}
		
		ActorsToIgnore.Add(HitActor);
	}

	return true;
}
