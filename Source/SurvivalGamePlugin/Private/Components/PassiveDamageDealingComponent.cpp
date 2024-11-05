// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PassiveDamageDealingComponent.h"

#include "Interfaces/HittableInterface.h"


UPassiveDamageDealingComponent::UPassiveDamageDealingComponent()
{
	OnComponentBeginOverlap.AddDynamic(this, &UPassiveDamageDealingComponent::OnActorBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UPassiveDamageDealingComponent::OnActorEndOverlap);
}

void UPassiveDamageDealingComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		IHittableInterface* HittableActor = Cast<IHittableInterface>(OverlappingActor);
		if (HittableActor != nullptr && OverlappingActor != GetOwner() && OverlappedActors.Contains(HittableActor) == false)
		{
			OverlappedActors.Add(HittableActor);
		}
	}
}

void UPassiveDamageDealingComponent::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	IHittableInterface* HittableActor = Cast<IHittableInterface>(OtherActor);
	
	if (HittableActor != nullptr)
	{
		OverlappedActors.Add(HittableActor);
		HittableActor->OnHit(InstantGameplayEffectSpecHandles);
	}
}

void UPassiveDamageDealingComponent::OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IHittableInterface* HittableActor = Cast<IHittableInterface>(OtherActor);
	if (HittableActor != nullptr)
	{
		OverlappedActors.Remove(HittableActor);
	}
}

void UPassiveDamageDealingComponent::ApplyInstantGameplayEffectSpecHandle(FGameplayEffectSpecHandle SpecHandle, bool bShouldApplyOnOverlap)
{
	TArray<FGameplayEffectSpecHandle> SpecHandlesToApply;
	SpecHandlesToApply.Add(SpecHandle);

	for (IHittableInterface* OverlappedActor : OverlappedActors)
	{
		OverlappedActor->OnHit(SpecHandlesToApply);
	}

}