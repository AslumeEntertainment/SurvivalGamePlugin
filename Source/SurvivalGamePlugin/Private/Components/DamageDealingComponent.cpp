// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DamageDealingComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Interfaces/HittableInterface.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_GameplayEffectContext.h"
#include "GAS/SG_AbilitySystemGlobals.h"

UDamageDealingComponent::UDamageDealingComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageDealingComponent::BeginPlay()
{
	Super::BeginPlay();
	AddActorToIgnored(GetOwner());
	DefaultGameplayEffectSpecHandle = ConfigDamageGameplayEffect(DefaultDamageEffect);
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

	if (UKismetSystemLibrary::CapsuleTraceMulti(World, GetComponentLocation(), GetComponentLocation(), DamageCapsuleRadius, DamageCapsuleHeight / 2, UEngineTypes::ConvertToTraceType(ECC_Camera),
		true, ActorsToIgnoreWhenActive, EDrawDebugTrace::ForOneFrame, HitResults, true) == false)
	{
		return;
	}

	for (const FHitResult HitResult : HitResults)
	{
		if (ActorsToIgnoreWhenActive.Contains(HitResult.GetActor()))
		{
			continue;
		}

		IHittableInterface* HittableActor = Cast<IHittableInterface>(HitResult.GetActor());
		if (HittableActor == nullptr)
		{
			continue;
		}
		if (HittableActor != nullptr && HittableActor->bCanBeHit() == true)
		{
			UAbilitySystemComponent* ASC = USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
			if (IsValid(ASC) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(ASC) == false"));
				continue;
			}

			FGameplayCueParameters CueParams = FGameplayCueParameters();
			CueParams.Location = HitResult.ImpactPoint;
			ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Hit.Regular"), CueParams);

			HittableActor->OnHit(GameplayEffectSpecHandles);
			OnHitLandedSuccessfully.Broadcast(HitResult.GetActor());
			ActorsToIgnoreWhenActive.Add(HitResult.GetActor());
		}
	}
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

void UDamageDealingComponent::ChangeDefaultDamageEffect(FDamageGameplayEffect NewGameplayEffect)
{
	DefaultDamageEffect = NewGameplayEffect;
	DefaultGameplayEffectSpecHandle = ConfigDamageGameplayEffect(DefaultDamageEffect);
}

void UDamageDealingComponent::SetDefaultDamageType(ESG_DamageType NewDamageType)
{
	if (DefaultGameplayEffectSpecHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::SetDefaultDamageType Damage Dealing Component #%s does not have a default damage effect!"), *DamageComponentTag.ToString());
		return;
	}
	DefaultDamageEffect.DamageType = NewDamageType;
	
	FGameplayEffectContextHandle EffectContextHandle = DefaultGameplayEffectSpecHandle.Data.Get()->GetContext();
	FSG_GameplayEffectContext* Context = static_cast<FSG_GameplayEffectContext*>(EffectContextHandle.Get());
	Context->SetDamageType(DefaultDamageEffect.DamageType);
	DefaultGameplayEffectSpecHandle.Data.Get()->SetContext(EffectContextHandle);
}

void UDamageDealingComponent::SetDefaultDamageValue(float NewValue)
{
	if (DefaultGameplayEffectSpecHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::SetDefaultDamageValue Damage Dealing Component #%s does not have a default damage effect!"), *DamageComponentTag.ToString());
		return;
	}
	DefaultDamageEffect.DamageValue = NewValue;
	DefaultGameplayEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(DefaultDamageEffect.SetByCallerMagnitudeDataTag, -DefaultDamageEffect.DamageValue);
}

void UDamageDealingComponent::SetDefaultDamageEffectDuration(float NewValue)
{
	if (DefaultGameplayEffectSpecHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::SetDefaultDamageEffectDuration Damage Dealing Component #%s does not have a default damage effect!"), *DamageComponentTag.ToString());
		return;
	}
	DefaultDamageEffect.DamageEffectDuration = NewValue;
	DefaultGameplayEffectSpecHandle.Data.Get()->SetDuration(DefaultDamageEffect.DamageEffectDuration, false);
}

void UDamageDealingComponent::SetDefaultDamageEffectPeriod(float NewValue)
{
	if (DefaultGameplayEffectSpecHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::SetDefaultDamageEffectPeriod Damage Dealing Component #%s does not have a default damage effect!"), *DamageComponentTag.ToString());
		return;
	}
	DefaultDamageEffect.DamageEffectPeriod = NewValue;
	DefaultGameplayEffectSpecHandle.Data.Get()->Period = DefaultDamageEffect.DamageEffectPeriod;
}

void UDamageDealingComponent::SetDamageComponentTagSuffix(FString Suffix)
{
	if (DamageComponentTag.ToString().Contains(Suffix))
	{
		return;
	}
	FString NewTag = DamageComponentTag.ToString() + Suffix;
	DamageComponentTag = DamageComponentTag.RequestGameplayTag(FName(NewTag));
}

void UDamageDealingComponent::ResetDamageComponentTagSuffix()
{
	DamageComponentTag = DamageComponentTag.RequestDirectParent();
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

	GameplayEffectSpecHandles.Add(EffectSpecHandle);
	return EffectSpecHandle;
}

void UDamageDealingComponent::ResetDamageEffects()
{
	GameplayEffectSpecHandles.Empty();
	DefaultGameplayEffectSpecHandle = ConfigDamageGameplayEffect(DefaultDamageEffect);
}

FGameplayTag UDamageDealingComponent::GetDamageComponentTag()
{
	return DamageComponentTag;
}
