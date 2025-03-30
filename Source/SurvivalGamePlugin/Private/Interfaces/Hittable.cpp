// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/Hittable.h"

#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"


void IHittable::OnComponentHitReceived(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* Self = Cast<AActor>(this);
	if (IsValid(Self) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("IHittable::OnComponentHitReceived IsValid(Self) == false"));
		return;
	}

	if (Cast<UMeshComponent>(OtherComp) == nullptr)
	{
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("hit %s at %s"), *OtherActor->GetName(), *OtherComp->GetName());
	//UE_LOG(LogTemp, Log, TEXT("UHittableComponent::OnImpactReceived - %s, Velocity - %.3f"), *OtherActor->GetName(), Self->GetVelocity().Length());

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag("CommonEvents.ReceiveDamage.Impact");
	FGameplayEventData EventData = FGameplayEventData();
	EventData.Instigator = OtherActor;
	EventData.Target = Self;

	USG_AbilitySystemBlueprintLibrary::SendGameplayEventToActor(Self, EventTag, EventData);
}

USG_AbilitySystemComponent* IHittable::GetSG_AbilitySystemComponent() const
{
	const AActor* Self = Cast<AActor>(this);
	if (IsValid(Self) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("IHittable::GetSG_AbilitySystemComponent IsValid(Self) == false"));
		return nullptr;
	}

	return Self->GetComponentByClass<USG_AbilitySystemComponent>();
}

bool IHittable::IsGuarding() const
{
	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(this);
	if (ASI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("IHittable::IsGuarding ASI == nullptr"));
		return false;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();

	return ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Bool.HasState.Guard")));
}

bool IHittable::IsHitGuarded(FVector HitLocation) const
{
	return false;
}

TArray<FHitResult> IHittable::CastBoxTraceAroundBounds(TArray<AActor*> ActorsToIgnore, float AdditionalExtendFromBounds, bool bShouldDrawDebug) const
{
	TArray<FHitResult> HitResults;

	const AActor* Self = Cast<AActor>(this);
	if (IsValid(Self) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("IHittable::CastBoxTraceAroundBounds IsValid(Self) == false"));
		return HitResults;
	}
	USceneComponent* Root = Self->GetRootComponent();
	if (IsValid(Root) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("IHittable::CastBoxTraceAroundBounds IsValid(Root) == false"));
		return HitResults;
	}

	UWorld* World = Self->GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("IHittable::CastBoxTraceAroundBounds IsValid(World) == false"));
		return HitResults;
	}

	FVector MinLocalBounds = Root->GetLocalBounds().GetBox().Min;
	FVector MaxLocalBounds = Root->GetLocalBounds().GetBox().Max;

	FVector ActorBottom = Self->GetActorLocation() + Self->GetActorUpVector() * MinLocalBounds.Z;
	FVector ActorTop = Self->GetActorLocation() + Self->GetActorUpVector() * MaxLocalBounds.Z;

	FVector TraceCenter = (ActorTop + ActorBottom) / 2;
	FVector TraceExtent = Root->GetLocalBounds().GetBox().GetExtent() + AdditionalExtendFromBounds;

	//UE_LOG(LogTemp, Log, TEXT("IHittable::CastBoxTraceAroundBounds: TraceCenter - %s, TraceExtend - %s"), *TraceCenter.ToString(), *TraceExtent.ToString());

	UKismetSystemLibrary::BoxTraceMulti(World, TraceCenter, TraceCenter, TraceExtent, Self->GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2,
		true, ActorsToIgnore, bShouldDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, HitResults, true);
	

	return HitResults;
}
