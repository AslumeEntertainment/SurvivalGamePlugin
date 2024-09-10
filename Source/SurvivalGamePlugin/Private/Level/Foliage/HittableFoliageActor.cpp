// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/Foliage/HittableFoliageActor.h"
#include "Components/StaticMeshComponent.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"


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
	UE_LOG(LogTemp, Warning, TEXT("AHittableFoliageActor::OnHit Actor was hit"));
	for (FGameplayEffectSpecHandle SpecHandle : GameplayEffectSpecHandles)
	{
		UE_LOG(LogTemp, Warning, TEXT("AHittableFoliageActor::OnHit Damage effect is applied"));
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AHittableFoliageActor::OnHittableDestroyed_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AHittableFoliageActor::OnHittableDestroyed_Implementation Actor was destroyed"));
	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		if (!IsValid(World))
		{
			UE_LOG(LogTemp, Error, TEXT("AHittableFoliageActor::OnHittableDestroyed_Implementation IsValid(World) == false"));
			return;
		}

		FVector MinBounds, MaxBounds;
		StaticMesh->GetLocalBounds(MinBounds, MaxBounds);
		double StartingHeight = (GetActorLocation().Z + MinBounds.Z) + ((MaxBounds.Z - MinBounds.Z) * StartingHeightPercent);
		FVector CurrentSpawningPoint = GetActorLocation();
		CurrentSpawningPoint.Z = StartingHeight;
		//UE_LOG(LogTemp, Warning, TEXT("Actor is located at %s and has the following bounds: %s ; %s"), *GetActorLocation().ToString(), *MinBounds.ToString(), *MaxBounds.ToString());

		for (auto& Drop : ActorsToDropOnDestroyed)
		{
			int Min = UKismetStringLibrary::Conv_StringToInt(Drop.Value.LeftChop(3));
			int Max = UKismetStringLibrary::Conv_StringToInt(Drop.Value.RightChop(4));
			int Count = FMath::RandRange(Min, Max);

			for (int i = 0; i < Count; i++)
			{
				AActor* DroppedActor = World->SpawnActor<AActor>(Drop.Key, GetActorLocation(), GetActorRotation() + UKismetMathLibrary::RandomRotator() * RotationLimit);
				FVector NewActorLocation = GetActorLocation();
				
				if (bShouldStackVertically)
				{
					NewActorLocation = CurrentSpawningPoint;
					UStaticMeshComponent* DroppedActorMesh = DroppedActor->GetComponentByClass<UStaticMeshComponent>();
					if (!IsValid(DroppedActorMesh))
					{
						continue;
					}
					FVector NewActorMinBounds, NewActorMaxBounds;
					StaticMesh->GetLocalBounds(NewActorMinBounds, NewActorMaxBounds);
					CurrentSpawningPoint += GetActorUpVector() * (NewActorMaxBounds.Z - NewActorMinBounds.Z);
				}
				else
				{
					NewActorLocation.Z += FMath::RandRange(StartingHeight, GetActorLocation().Z + (MaxBounds.Z - MinBounds.Z));
				}
				NewActorLocation.X += FMath::RandRange(MinBounds.X, MaxBounds.X) * Spread;
				NewActorLocation.Y += FMath::RandRange(MinBounds.Y, MaxBounds.Y) * Spread;

				DroppedActor->SetActorLocation(NewActorLocation);
			}
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


