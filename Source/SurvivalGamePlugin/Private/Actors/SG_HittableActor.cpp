// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SG_HittableActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Components/LootTableComponent.h"


ASG_HittableActor::ASG_HittableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(StaticMeshComponent);

	CollisionCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CollisionCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionCapsuleComponent->SetupAttachment(StaticMeshComponent);
	CollisionCapsuleComponent->SetGenerateOverlapEvents(true);
	CollisionCapsuleComponent->OnComponentHit.AddDynamic(this, &ASG_HittableActor::OnComponentHitReceived);
	

	LootTableComponent = CreateDefaultSubobject<ULootTableComponent>(TEXT("LootTable"));

	AbilitySystemComponent = CreateDefaultSubobject<USG_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USG_AttributeSet>(TEXT("AttributeSet"));

}

void ASG_HittableActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)
	{
		return;
	}

	NetMulticast_SetActorPhysics(bStartWithPhysicsEnabled);

	if (IsValid(InitializingEffect))
	{
		AbilitySystemComponent->AddEffectFromClass(InitializingEffect, this, this, this);
	}
	AbilitySystemComponent->SetStartingAttributeValues();
}

void ASG_HittableActor::Server_SetActorPhysics_Implementation(bool NewActorPhysics)
{
	NetMulticast_SetActorPhysics(NewActorPhysics);
}

void ASG_HittableActor::NetMulticast_SetActorPhysics_Implementation(bool NewActorPhysics)
{
	StaticMeshComponent->SetSimulatePhysics(NewActorPhysics);

	ECollisionEnabled::Type CollisionState = ECollisionEnabled::NoCollision;

	if (NewActorPhysics || (NewActorPhysics == false && bHasCollisionWhenPhysicsDisabled))
	{
		CollisionState = ECollisionEnabled::QueryAndPhysics;
	}
	
	StaticMeshComponent->SetCollisionEnabled(CollisionState);
	//CollisionCapsuleComponent->SetCollisionEnabled(CollisionState);
}

UShapeComponent* ASG_HittableActor::GetImpactChecker()
{
	return CollisionCapsuleComponent;
}

void ASG_HittableActor::OnComponentBeginTouch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IHittable* Hit = Cast<IHittable>(OtherActor);
	bool TouchingHittable = Hit == nullptr;
	UE_LOG(LogTemp, Warning, TEXT("ASG_HittableActor::OnComponentBeginTouch - %s   --- ovelapping with %s"), *GetName(), *OtherActor->GetName());
}

UAbilitySystemComponent* ASG_HittableActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UStaticMeshComponent* ASG_HittableActor::GetStaticMeshComponent()
{
	return StaticMeshComponent;
}

ULootTableComponent* ASG_HittableActor::GetLootTableComponent()
{
	return LootTableComponent;
}

void ASG_HittableActor::DestroyHittable(bool bSafeDestruct)
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_HittableActor::DestroyHittable IsValid(AbilitySystemComponent) == false"));
		return;
	}
	AbilitySystemComponent->OnOwnerDestroyed();

	if (HasAuthority() == false)
	{
		return;
	}

	LootTableComponent->DropLoot(bSafeDestruct);
	Destroy();
}

USceneComponent* ASG_HittableActor::GetBoundsComponent()
{
	return CollisionCapsuleComponent;
}

void ASG_HittableActor::SetActorStatic()
{
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASG_HittableActor::Server_SetActorStatic_Implementation()
{
	NetMulticast_SetActorStatic();
}

void ASG_HittableActor::NetMulticast_SetActorStatic_Implementation()
{
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}



