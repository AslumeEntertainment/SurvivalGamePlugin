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
