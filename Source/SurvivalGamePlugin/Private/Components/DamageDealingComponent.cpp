// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DamageDealingComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Components/HittableComponent.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_GameplayEffectContext.h"
#include "GAS/SG_AbilitySystemGlobals.h"
#include "Objects/DamageSource.h"


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
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(World) == false"));
		return;
	}

	TArray<FHitResult> HitResults;

	/*if (UKismetSystemLibrary::CapsuleTraceMulti(World, GetComponentLocation(), GetComponentLocation(), DamageCapsuleRadius, DamageCapsuleHeight / 2, UEngineTypes::ConvertToTraceType(ECC_Camera),
		true, ActorsToIgnoreWhenActive, EDrawDebugTrace::ForOneFrame, HitResults, true) == false)
	{
		return;
	}*/

	for (const FHitResult HitResult : HitResults)
	{
		if (ActorsToIgnoreWhenActive.Contains(HitResult.GetActor()))
		{
			continue;
		}

		UHittableComponent* HittableComp = HitResult.GetActor()->GetComponentByClass<UHittableComponent>();
		if (IsValid(HittableComp) == false)
		{
			continue;
		}
		if (HittableComp->CanEverReceiveGameplayEffects() == false)
		{
			return;
		}

		USG_AbilitySystemComponent* ASC = HittableComp->GetOwnerAbilitySystemComponent();
		if (IsValid(ASC) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(ASC) == false"));
			continue;
		}

		FGameplayCueParameters CueParams = FGameplayCueParameters();
		CueParams.Location = HitResult.ImpactPoint;
		ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Hit.Regular"), CueParams);

		//HittableComp->OnHit(GameplayEffectSpecHandles);
		OnHitLandedSuccessfully.Broadcast(HitResult.GetActor());
		ActorsToIgnoreWhenActive.Add(HitResult.GetActor());
	}
}

TMap<FGameplayTag, UDamageSource*> UDamageDealingComponent::GetDamageSources()
{
	return DamageSources;
}

void UDamageDealingComponent::AddDamageSource(UDamageSource* DamageSource, FGameplayTag DamageSourceTag)
{
	if (DamageSources.Contains(DamageSourceTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::AddDamageSource There is already a damage source with tag %s"), *DamageSourceTag.ToString());
		return;
	}
	DamageSources.Add(DamageSourceTag, DamageSource);
}

void UDamageDealingComponent::RemoveDamageSource(FGameplayTag DamageSourceTag)
{
	if (DamageSources.Contains(DamageSourceTag) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::RemoveDamageSource DamageSources.Contains(DamageSourceTag) == false"));
		return;
	}
	DamageSources.Remove(DamageSourceTag);
}

void UDamageDealingComponent::SetDamageSourceActivation(FGameplayTag DamageSourceTag, bool bShouldActivate)
{
	UDamageSource* DamageSource = DamageSources.FindRef(DamageSourceTag);
	if (IsValid(DamageSource) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::SetDamageSourceActivation IsValid(DamageSource) == false"));
		return;
	}
	//add source to active sources
}

void UDamageDealingComponent::SetActivation(bool NewState)
{
	if (GetOwner()->HasAuthority() == false)
	{
		return;
	}
	if (NewState)
	{
		ActorsToIgnoreWhenActive.Empty();
		ActorsToIgnoreWhenActive = ActorsToIgnore;
	}

	SetComponentTickEnabled(NewState);
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

FGameplayEffectSpecHandle UDamageDealingComponent::ConfigDamageGameplayEffect(FDamageGameplayEffect NewGameplayEffect)
{
	if (IsValid(NewGameplayEffect.DamageEffectClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::ConfigDamageGameplayEffect IsValid(NewGameplayEffect.DamageEffectClass) == false"));
		return FGameplayEffectSpecHandle();
	}
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::ConfigDamageGameplayEffect IsValid(GetOwner()) == false"));
		return FGameplayEffectSpecHandle();
	}

	FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(USG_AbilitySystemGlobals::Get().AllocGameplayEffectContext());
	FSG_GameplayEffectContext* Context = static_cast<FSG_GameplayEffectContext*>(EffectContextHandle.Get());
	Context->SetDamageType(NewGameplayEffect.DamageType);
	EffectContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpecHandle = UAbilitySystemBlueprintLibrary::MakeSpecHandle(NewGameplayEffect.DamageEffectClass.GetDefaultObject(), GetOwner(), GetOwner());
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();

	EffectSpec->SetContext(EffectContextHandle);
	EffectSpec->SetDuration(NewGameplayEffect.DamageEffectDuration, false);
	EffectSpec->Period = NewGameplayEffect.DamageEffectPeriod;
	EffectSpec->SetSetByCallerMagnitude(NewGameplayEffect.SetByCallerMagnitudeDataTag, -NewGameplayEffect.DamageValue);

	return EffectSpecHandle;
}

