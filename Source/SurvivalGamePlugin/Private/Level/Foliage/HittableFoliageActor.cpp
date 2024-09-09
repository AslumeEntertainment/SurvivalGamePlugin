// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/Foliage/HittableFoliageActor.h"
#include "Components/StaticMeshComponent.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetStringLibrary.h"


AHittableFoliageActor::AHittableFoliageActor()
{
	bReplicates = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USG_AttributeSet>(TEXT("AttributeSet"));
}

void AHittableFoliageActor::BeginPlay()
{
	Super::BeginPlay();

	AttributeSet->SetMaxHealth(MaxHealth);
	AttributeSet->SetHealth(MaxHealth);
}

void AHittableFoliageActor::OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles)
{
	if (HasAuthority() == false)
	{
		return;
	}
	for (FGameplayEffectSpecHandle SpecHandle : GameplayEffectSpecHandles)
	{
		UE_LOG(LogTemp, Warning, TEXT("AHittableFoliageActor::OnHit Actor was hit"));
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AHittableFoliageActor::OnHittableDestroyed_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AHittableFoliageActor::OnHittableDestroyed_Implementation Actor was destroyed"));
	if (HasAuthority())
	{
		for (auto& Drop : ActorsToDropOnDestroyed)
		{
			FString MinStr = Drop.Value.LeftChop(3);
			FString MaxStr = Drop.Value.RightChop(4);

			int Min = UKismetStringLibrary::Conv_StringToInt(MinStr);
			int Max = UKismetStringLibrary::Conv_StringToInt(MaxStr);
			int Count = FMath::RandRange(Min, Max);
			UE_LOG(LogTemp, Warning, TEXT("Going to drop %d actors of type"), Count);
		}
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


