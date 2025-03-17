// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/Hittable.h"

#include "GameplayTagContainer.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"


void IHittable::OnComponentHitReceived(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* Self = Cast<AActor>(this);
	if (IsValid(Self) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("IHittable::OnComponentHitReceived IsValid(Self) == false"));
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

	USG_AbilitySystemBlueprintLibrary::SendGameplayEventToActor(Self, EventTag, EventData);
}
