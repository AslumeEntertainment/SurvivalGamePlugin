// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/GameplayEffectComponents/SpreadGameplayEffectComponent.h"

#include "Kismet/KismetSystemLibrary.h"

#include "Objects/GameplayEffectDataObjects/GameplayEffectData.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interfaces/Hittable.h"


USpreadGameplayEffectComponent::USpreadGameplayEffectComponent()
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Spread Gameplay Effect while active");
#endif
}

void USpreadGameplayEffectComponent::OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(ActiveGEContainer.Owner);
	if (IsValid(TargetASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USpreadGameplayEffectComponent::OnGameplayEffectExecuted IsValid(TargetASC) == false"));
		return;
	}

	if (TargetASC->GetNumericAttribute(USG_AttributeSet::GetCurrentHealthAttribute()) <= 0.f)
	{
		return;
	}

	AActor* Target = TargetASC->GetOwner();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USpreadGameplayEffectComponent::OnGameplayEffectExecuted IsValid(Target) == false"));
		return;
	}
	IHittable* HittableTarget = Cast<IHittable>(Target);
	if (HittableTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USpreadGameplayEffectComponent::OnGameplayEffectExecuted HittableTarget == nullptr"));
		return;
	}

	UWorld* World = Target->GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USpreadGameplayEffectComponent::OnGameplayEffectExecuted IsValid(World) == false"));
		return;
	}

	USceneComponent* BoundsComponnent = HittableTarget->GetBoundsComponent();
	if (IsValid(BoundsComponnent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USpreadGameplayEffectComponent::OnGameplayEffectExecuted IsValid(BoundsComponnent) == false"));
		return;
	}

	TArray<AActor*> ActorsToIgnore;
	if (bShouldIgnoreSelf) ActorsToIgnore.Add(Target);

	TArray<FHitResult> HitResults;

	FVector MinLocalBounds = BoundsComponnent->GetLocalBounds().GetBox().Min;
	FVector MaxLocalBounds = BoundsComponnent->GetLocalBounds().GetBox().Max;

	FVector ActorBottom = Target->GetActorLocation() + Target->GetActorUpVector() * MinLocalBounds.Z;
	FVector ActorTop = Target->GetActorLocation() + Target->GetActorUpVector() * MaxLocalBounds.Z;

	FVector TraceCenter = (ActorTop + ActorBottom) / 2;
	FVector TraceExtent = BoundsComponnent->GetLocalBounds().GetBox().GetExtent() + ExtentFromTargetBounds;

	if(bShouldDrawDebug) UE_LOG(LogTemp, Log, TEXT("USpreadGameplayEffectComponent: TraceCenter - %s, TraceExtend - %s"), *TraceCenter.ToString(), *TraceExtent.ToString());

	UKismetSystemLibrary::BoxTraceMulti(World, TraceCenter, TraceCenter, TraceExtent, Target->GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2,
		true, ActorsToIgnore, bShouldDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, HitResults, true);

	if (HitResults.IsEmpty())
	{
		return;
	}

	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();

	for (const FHitResult HitResult : HitResults)
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

		if (IsValid(GameplayEffectToSpread))
		{
			if (IsValid(GameplayEffectToSpread->EffectClass) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("USpreadGameplayEffectComponent::OnGameplayEffectExecuted IsValid(GameplayEffectToSpread->EffectClass) == false"));
				return;
			}

			ContextHandle.AddSourceObject(GameplayEffectToSpread);
			SpecHandle = HitActorASC->MakeOutgoingSpec(GameplayEffectToSpread->EffectClass, GameplayEffectToSpread->EffectLevel, ContextHandle);
		}
		else
		{
			const UGameplayEffectData* ActiveGameplayEffectData = Cast<UGameplayEffectData>(GESpec.GetContext().GetSourceObject());

			if (IsValid(ActiveGameplayEffectData))
			{
				UE_LOG(LogTemp, Error, TEXT("USpreadGameplayEffectComponent::OnGameplayEffectExecuted IsValid(ActiveGameplayEffectData) == false"));
				return;
			}
			if (IsValid(ActiveGameplayEffectData->EffectClass) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("USpreadGameplayEffectComponent::OnGameplayEffectExecuted IsValid(ActiveGameplayEffectData->EffectClass) == false"));
				return;
			}

			ContextHandle.AddSourceObject(ActiveGameplayEffectData);
			SpecHandle = HitActorASC->MakeOutgoingSpec(ActiveGameplayEffectData->EffectClass, ActiveGameplayEffectData->EffectLevel, ContextHandle);
		}

		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (Spec == nullptr)
		{
			continue;
		}

		if (bShouldDrawDebug) UE_LOG(LogTemp, Warning, TEXT("USpreadGameplayEffectComponent: Spreading effect from %s to %s"), *Target->GetName(), *HitActor->GetName());

		HitActorASC->ApplyGameplayEffectSpecToSelf(*Spec);
		ActorsToIgnore.Add(HitActor);
	}
}
