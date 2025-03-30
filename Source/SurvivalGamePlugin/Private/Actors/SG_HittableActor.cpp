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
	CollisionCapsuleComponent->SetGenerateOverlapEvents(true);
	CollisionCapsuleComponent->SetupAttachment(StaticMeshComponent);

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

	if (IsValid(InitializingEffect))
	{
		AbilitySystemComponent->AddEffectFromClass(InitializingEffect, this, this, this);
	}
	AbilitySystemComponent->SetStartingAttributeValues();
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



