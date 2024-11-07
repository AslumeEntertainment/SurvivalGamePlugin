// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/Foliage/HittableFoliageActor.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Components/PassiveDamageDealingComponent.h"
#include "Components/LootTableComponent.h"


AHittableFoliageActor::AHittableFoliageActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	PassiveDamageDealingComponent = CreateDefaultSubobject<UPassiveDamageDealingComponent>(TEXT("PassiveDamageDealingComponent"));
	PassiveDamageDealingComponent->SetupAttachment(GetRootComponent());

	LootTableComponent = CreateDefaultSubobject<ULootTableComponent>(TEXT("LootTable"));

	AbilitySystemComponent = CreateDefaultSubobject<USG_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USG_AttributeSet>(TEXT("AttributeSet"));
}

void AHittableFoliageActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)
	{
		return;
	}

	AbilitySystemComponent->AddEffectFromClass(DefaultAttributeEffect, this);
}

void AHittableFoliageActor::OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles)
{
	for (FGameplayEffectSpecHandle SpecHandle : GameplayEffectSpecHandles)
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AHittableFoliageActor::OnHittableDestroyed()
{
	if (HasAuthority())
	{
		LootTableComponent->DropLoot();
	}

	Destroy();
}

bool AHittableFoliageActor::bCanBeHit()
{
	return true;
}

UAbilitySystemComponent* AHittableFoliageActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


