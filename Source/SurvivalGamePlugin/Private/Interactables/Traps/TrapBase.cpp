// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Traps/TrapBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

#include "Components/DamageDealingComponent.h"
#include "Components/HittableComponent.h"

ATrapBase::ATrapBase()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMeshComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATrapBase::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ATrapBase::OnBoxEndOverlap);
	BoxComponent->SetupAttachment(StaticMeshComponent);

	DamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("DamageSource"));
	DamageDealingComponent->SetupAttachment(BoxComponent);
}

void ATrapBase::BeginPlay()
{
	Super::BeginPlay();

	DamageDealingComponent->AddActorToIgnored(this);
	DamageDealingComponent->ConfigDamageSource(this);
	
}

void ATrapBase::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor->GetComponentByClass<UHittableComponent>()) == false)
	{
		return;
	}

	DamageDealingComponent->SetActiveness(true);

	if (DamageDealingComponent->ShouldOnlyHitOnce() == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ATrapBase::OnBoxBeginOverlap IsValid(World) == false"));
		return;
	}

	FTimerHandle DamageSourceDeactivationTimer;
	FTimerDelegate DamageSourceDeactivationTimerDelegate;
	DamageSourceDeactivationTimerDelegate.BindUFunction(DamageDealingComponent, "SetActiveness", false);
	World->GetTimerManager().SetTimer(DamageSourceDeactivationTimer, DamageSourceDeactivationTimerDelegate, 0.5f, false);
}

void ATrapBase::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (DamageDealingComponent->ShouldOnlyHitOnce())
	{
		return;
	}
	if (IsBoxStillOverlapping() == false)
	{
		DamageDealingComponent->SetActiveness(false);
	}
}

bool ATrapBase::IsBoxStillOverlapping()
{
	TArray<AActor*> OverlappingActors;

	BoxComponent->GetOverlappingActors(OverlappingActors);
	if (OverlappingActors.IsEmpty())
	{
		return true;
	}

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (IsValid(OverlappingActor->GetComponentByClass<UHittableComponent>()))
		{
			return true;
		}
	}
	return false;
}



