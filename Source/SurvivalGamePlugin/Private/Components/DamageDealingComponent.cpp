// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DamageDealingComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemGlobals.h"

#include "Objects/GameplayEffectDataObjects/DamageSourceData.h"
#include "Objects/TraceEmitters/SG_TraceEmitterBase.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"


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
		if (ActorsToIgnoreWhenActive.Contains(HitActor))
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

		ContextHandle.AddHitResult(HitResult, true);
		
		NetMulticast_ShowOnHitEffect(HitResult.ImpactPoint);

		for (UGameplayEffectData* GameplayEffectData : GameplayEffectsData)
		{
			if (IsValid(GameplayEffectData) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(GameplayEffectData) == false"));
				continue;
			}
			if (IsValid(GameplayEffectData->EffectClass) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(GameplayEffectData->DamageEffect) == false"));
				continue;
			}
			ContextHandle.AddSourceObject(GameplayEffectData);
			
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectData->EffectClass, GameplayEffectData->EffectLevel, ContextHandle);
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec == nullptr)
			{
				continue;
			}

			TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
		}

		OnHitLandedSuccessfully.Broadcast(HitResult.GetActor());

		if (bShouldOnlyHitOnce) ActorsToIgnoreWhenActive.Add(HitResult.GetActor());
	}
}

void UDamageDealingComponent::ShowOnHitEffect(FVector Location)
{
	if (IsValid(OnHitLandedEffect) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: IsValid(OnHitLandedEffect) == false"), *GetName());
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::ShowOnHitEffect IsValid(World) == false"));
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, OnHitLandedEffect, Location, GetOwner()->GetActorRotation(), GetOwner()->GetActorScale3D());
}

void UDamageDealingComponent::NetMulticast_ShowOnHitEffect_Implementation(FVector Location)
{
	ShowOnHitEffect(Location);
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

UDamageSourceData* UDamageDealingComponent::GetDamageSourceData()
{
	if (GameplayEffectsData.IsEmpty())
	{
		return nullptr;
	}

	UDamageSourceData* DamageSourceData = Cast<UDamageSourceData>(GameplayEffectsData[0]);
	return DamageSourceData;
}

