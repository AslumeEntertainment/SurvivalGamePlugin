// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DamageDealingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HittableInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"

UDamageDealingComponent::UDamageDealingComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageDealingComponent::AttemptHit_Implementation()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::AttemptHit IsValid(World) == false"));
		return;
	}
	
	TArray<FHitResult> HitResults;

	UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::AttemptHit Number of actors to currently ignore = %d"), ActorsToIgnoreWhenActive.Num());

	if (UKismetSystemLibrary::CapsuleTraceMulti(World, GetComponentLocation(), GetComponentLocation(), DamageCapsuleRadius, DamageCapsuleHeight / 2, UEngineTypes::ConvertToTraceType(ECC_Camera),
		true, ActorsToIgnoreWhenActive, EDrawDebugTrace::ForOneFrame, HitResults, true) == false)
	{
		return;
	}

	for (FHitResult HitResult : HitResults)
	{
		if (ActorsToIgnoreWhenActive.Contains(HitResult.GetActor()))
		{
			continue;
		}
		IHittableInterface* HittableActor = Cast<IHittableInterface>(HitResult.GetActor());
		IHittableInterface* HittableComponent = Cast<IHittableInterface>(HitResult.GetComponent());

		if (HittableActor == nullptr && HittableComponent == nullptr)
		{
			continue;
		}
		if (HittableActor != nullptr && HittableActor->bCanBeHit() == true)
		{
			HittableActor->OnHit(GameplayEffectSpecHandles);
			OnHitLandedSuccessfully.Broadcast(HitResult.GetActor());
			AddActorToIgnored(HitResult.GetActor());
			//ActorsToIgnoreWhenActive.Add(HitResult.GetActor());
			continue;
		}
		if (HittableComponent != nullptr && HittableComponent->bCanBeHit() == true)
		{
			HittableComponent->OnHit(GameplayEffectSpecHandles);
			OnHitLandedSuccessfully.Broadcast(HitResult.GetComponent());
			RemoveActorFromIgnored(HitResult.GetActor());
			//ActorsToIgnoreWhenActive.Add(HitResult.GetActor());
			continue;
		}
	}
}

void UDamageDealingComponent::BeginPlay()
{
	Super::BeginPlay();
	AddActorToIgnored(GetOwner());
	//ActorsToIgnore.Add(GetOwner());
	DefaultGameplayEffectSpecHandle = ConfigDamageGameplayEffect(DefaultDamageEffect);
}

void UDamageDealingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AttemptHit();
}

//void UDamageDealingComponent::OnHitRegistered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (ActorsToIgnore.Contains(OtherActor))
//	{
//		return;
//	}
//
//	IHittableInterface* HittableActor = Cast<IHittableInterface>(OtherActor);
//	IHittableInterface* HittableComponent = Cast<IHittableInterface>(OtherComp);
//
//	if (HittableActor == nullptr && HittableComponent == nullptr)
//	{
//		return;
//	}
//	if (HittableActor != nullptr && HittableActor->bCanBeHit() == true)
//	{
//		HittableActor->OnHit(GameplayEffectSpecHandles);
//		OnHitLandedSuccessfully.Broadcast(OtherActor);
//		return;
//	}
//	if (HittableComponent != nullptr && HittableComponent->bCanBeHit() == true)
//	{
//		HittableComponent->OnHit(GameplayEffectSpecHandles);
//		OnHitLandedSuccessfully.Broadcast(OtherComp);
//		return;
//	}
//}

void UDamageDealingComponent::SetActivation_Implementation(bool NewState)
{
	if (NewState)
	{
		ActorsToIgnoreWhenActive.Empty();
		ActorsToIgnoreWhenActive = ActorsToIgnore;
		//OnComponentBeginOverlap.AddDynamic(this, &UDamageDealingComponent::OnHitRegistered);
	}
	else
	{
		//OnComponentBeginOverlap.Clear();
	}
	SetComponentTickEnabled(NewState);
}

void UDamageDealingComponent::AddActorToIgnored_Implementation(AActor* ActorToIgnore)
{
	if (ActorsToIgnore.Contains(ActorToIgnore) == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::AddActorToIgnored_Implementation Actor is already being ignored!"));
		return;
	}
	ActorsToIgnore.Add(ActorToIgnore);
}

void UDamageDealingComponent::RemoveActorFromIgnored_Implementation(AActor* ActorToUnignore)
{
	if (ActorsToIgnore.Contains(ActorToUnignore) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::RemoveActorFromIgnored_Implementation Actor is not being ignored!"));
		return;
	}
	ActorsToIgnore.Remove(ActorToUnignore);
}

void UDamageDealingComponent::SetDefaultDamageValue(float NewValue)
{
	if (DefaultGameplayEffectSpecHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::SetDefaultDamageValue Damage Dealing Component #%s does not have a default damage effect!"), *DamageComponentTag.ToString());
		return;
	}
	DefaultDamageEffect.DamageValue = NewValue;
	GameplayEffectSpecHandles[0].Data.Get()->SetSetByCallerMagnitude(DefaultDamageEffect.SetByCallerMagnitudeDataTag, -DefaultDamageEffect.DamageValue);
}

void UDamageDealingComponent::SetDefaultDamageEffectDuration(float NewValue)
{
	if (DefaultGameplayEffectSpecHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::SetDefaultDamageEffectDuration Damage Dealing Component #%s does not have a default damage effect!"), *DamageComponentTag.ToString());
		return;
	}
	DefaultDamageEffect.DamageEffectDuration = NewValue;
	GameplayEffectSpecHandles[0].Data.Get()->SetDuration(DefaultDamageEffect.DamageEffectDuration, false);
}

void UDamageDealingComponent::SetDefaultDamageEffectPeriod(float NewValue)
{
	if (DefaultGameplayEffectSpecHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageDealingComponent::SetDefaultDamageEffectPeriod Damage Dealing Component #%s does not have a default damage effect!"), *DamageComponentTag.ToString());
		return;
	}
	DefaultDamageEffect.DamageEffectPeriod = NewValue;
	GameplayEffectSpecHandles[0].Data.Get()->Period = DefaultDamageEffect.DamageEffectPeriod;
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
	if (!IsValid(NewGameplayEffect.DamageEffectClass))
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageDealingComponent::ConfigDamageGameplayEffect IsValid(NewGameplayEffect.DamageEffectClass) == false"));
		return FGameplayEffectSpecHandle();
	}
	FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpecHandle = UAbilitySystemBlueprintLibrary::MakeSpecHandle(NewGameplayEffect.DamageEffectClass.GetDefaultObject(), GetOwner(), GetOwner());
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();

	EffectSpec->SetContext(EffectContext);
	EffectSpec->SetDuration(NewGameplayEffect.DamageEffectDuration, false);
	EffectSpec->Period = NewGameplayEffect.DamageEffectPeriod;
	EffectSpec->SetSetByCallerMagnitude(NewGameplayEffect.SetByCallerMagnitudeDataTag, -NewGameplayEffect.DamageValue);

	GameplayEffectSpecHandles.Add(EffectSpecHandle);
	return EffectSpecHandle;
}

void UDamageDealingComponent::ResetDamageEffects()
{
	GameplayEffectSpecHandles.Empty();
	GameplayEffectSpecHandles.Add(DefaultGameplayEffectSpecHandle);
}

FGameplayTag UDamageDealingComponent::GetDamageComponentTag()
{
	return DamageComponentTag;
}
