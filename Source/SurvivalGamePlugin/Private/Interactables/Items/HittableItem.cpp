// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/HittableItem.h"

#include "Net/UnrealNetwork.h"

#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Components/LootTableComponent.h"

AHittableItem::AHittableItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	LootTableComponent = CreateDefaultSubobject<ULootTableComponent>(TEXT("LootTable"));

	AbilitySystemComponent = CreateDefaultSubobject<USG_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USG_AttributeSet>(TEXT("AttributeSet"));
}

void AHittableItem::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)
	{
		return;
	}

	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHittableItem::BeginPlay IsValid(AbilitySystemComponent) == false"));
		return;
	}
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHittableItem::BeginPlay IsValid(AttributeSet) == false"));
		return;
	}

	AbilitySystemComponent->AddEffectFromClass(DefaultAttributeEffect, this);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			SetDurability(Data.NewValue);
		}
	);
}

void AHittableItem::OnImpactReceived(AActor* OverlappedActor, AActor* OtherActor)
{
	// to be continued
}

void AHittableItem::LoseDurability(float LostDurability)
{
	Super::LoseDurability(LostDurability);

	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHittableItem::LoseDurability IsValid(AbilitySystemComponent) == false"));
		return;
	}
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHittableItem::LoseDurability IsValid(AttributeSet) == false"));
		return;
	}

	AbilitySystemComponent->SetNumericAttributeBase(AttributeSet->GetHealthAttribute(), CurrentDurability);
}

void AHittableItem::OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles)
{
	for (FGameplayEffectSpecHandle SpecHandle : GameplayEffectSpecHandles)
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AHittableItem::OnHittableDestroyed()
{
	if (HasAuthority())
	{
		LootTableComponent->DropLoot();
	}

	Destroy();
}

bool AHittableItem::bCanBeHit()
{
	return true;
}

UAbilitySystemComponent* AHittableItem::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
