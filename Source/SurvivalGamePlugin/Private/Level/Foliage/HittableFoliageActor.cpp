// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/Foliage/HittableFoliageActor.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"


AHittableFoliageActor::AHittableFoliageActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	AbilitySystemComponent = CreateDefaultSubobject<USG_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USG_AttributeSet>(TEXT("AttributeSet"));
}

void AHittableFoliageActor::BeginPlay()
{
	Super::BeginPlay();
}

void AHittableFoliageActor::OnImpactReceived(AActor* OverlappedActor, AActor* OtherActor)
{
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

		FVector MinLocalBounds, MaxLocalBounds;
		StaticMesh->GetLocalBounds(MinLocalBounds, MaxLocalBounds);
		FVector ActorSize = MaxLocalBounds - MinLocalBounds;
		FVector ActorBottom = GetActorLocation() + GetActorUpVector() * MinLocalBounds.Z;
		FVector ActorTop = GetActorLocation() + GetActorUpVector() * MaxLocalBounds.Z;

		double LowerHeight = ActorSize.Z * LowerHeightPercent;
		double UpperHeight = ActorSize.Z * LowerHeightPercent;

		FVector CurrentSpawningPoint = ActorBottom;
		CurrentSpawningPoint += GetActorUpVector() * (ActorSize.Z * LowerHeightPercent);

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
					DroppedActorMesh->GetLocalBounds(NewActorMinBounds, NewActorMaxBounds);
					CurrentSpawningPoint += GetActorUpVector() * (NewActorMaxBounds.Z - NewActorMinBounds.Z);
				}
				else
				{
					NewActorLocation.Z = ActorBottom.Z;
					NewActorLocation.Z += FMath::RandRange(LowerHeight, UpperHeight);
				}
				NewActorLocation.X += FMath::RandRange(MinLocalBounds.X, MaxLocalBounds.X) * Spread;
				NewActorLocation.Y += FMath::RandRange(MinLocalBounds.Y, MaxLocalBounds.Y) * Spread;

				DroppedActor->SetActorLocation(NewActorLocation, false, nullptr, ETeleportType::ResetPhysics);
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


