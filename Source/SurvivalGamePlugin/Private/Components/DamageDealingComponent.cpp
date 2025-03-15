// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DamageDealingComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Components/HittableComponent.h"
#include "Objects/SG_TraceEmitterBase.h"
#include "Objects/DamageSourceData.h"


UDamageDealingComponent::UDamageDealingComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageDealingComponent::BeginPlay()
{
	Super::BeginPlay();
	AddActorToIgnored(GetOwner());
}

void UDamageDealingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->HasAuthority())
	{
		AttemptHit();
	}
}

void UDamageDealingComponent::AttemptHit()
{
	if (IsValid(DamageTrace) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(DamageTrace) == false"));
		return;
	}
	
	TArray<FHitResult> HitResults = DamageTrace->CastTrace(GetComponentTransform(), ActorsToIgnoreWhenActive, GetUpVector());
	if (HitResults.IsEmpty())
	{
		return;
	}
	ProcessHit(HitResults);
}

void UDamageDealingComponent::ProcessHit(TArray<FHitResult> HitResults)
{
	FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
	ContextHandle.AddInstigator(DamageInstigator, GetOwner());

	for (const FHitResult HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) == false)
		{
			continue;
		}

		USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor));
		if (IsValid(TargetASC) == false)
		{
			continue;
		}
		if (TargetASC->bCanEverReceiveDamageFromEffects == false)
		{
			continue;
		}

		ContextHandle.AddOrigin(HitResult.ImpactPoint);
		
		FGameplayCueParameters CueParams = FGameplayCueParameters();
		CueParams.Location = HitResult.ImpactPoint;
		for (FGameplayTag GameplayCueTag : OnHitGameplayCueTags)
		{
			TargetASC->ExecuteGameplayCue(GameplayCueTag, CueParams);
		}

		for (UDamageSourceData* DamageEffectData : DamageEffectsData)
		{
			ContextHandle.AddSourceObject(DamageEffectData);
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectData->DamageEffect, DamageEffectData->EffectLevel, ContextHandle);
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		OnHitLandedSuccessfully.Broadcast(HitResult.GetActor());

		if (bShouldOnlyHitOnce) ActorsToIgnoreWhenActive.Add(HitResult.GetActor());
	}
}

void UDamageDealingComponent::ConfigDamageSource(AActor* InDamageInstigator)
{
	DamageInstigator = InDamageInstigator;
}

void UDamageDealingComponent::SetActiveness(bool NewState)
{
	if (GetOwner()->HasAuthority() == false)
	{
		return;
	}

	SetComponentTickEnabled(NewState);

	if (NewState)
	{
		ActorsToIgnoreWhenActive.Empty();
		ActorsToIgnoreWhenActive = ActorsToIgnore;
	}
}

void UDamageDealingComponent::AddActorToIgnored(AActor* ActorToIgnore)
{
	if (ActorsToIgnore.Contains(ActorToIgnore) == true)
	{
		return;
	}
	ActorsToIgnore.Add(ActorToIgnore);
}

void UDamageDealingComponent::RemoveActorFromIgnored(AActor* ActorToUnignore)
{
	ActorsToIgnore.Remove(ActorToUnignore);
}

