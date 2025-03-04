// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LootTableComponent.h"

#include "Components/MeshComponent.h"
#include "Kismet/KismetMathLibrary.h"


ULootTableComponent::ULootTableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULootTableComponent::DropLoot(bool bWasDestroyedSafely, bool bWasDestroyedUnsafely)
{
	if (bWasDestroyedSafely && bWasDestroyedUnsafely)
	{
		UE_LOG(LogTemp, Warning, TEXT("ULootTableComponent::DropLoot bWasDestroyedSafely and bWasDestroyedUnsafely are both true"));
		return;
	}

	FLootTableData* DropData = nullptr;
	if (bWasDestroyedSafely)
	{
		DropData = SafeDestructLootTable.GetRow<FLootTableData>("");
	}
	else if (bWasDestroyedUnsafely)
	{
		DropData = UnsafeDestructLootTable.GetRow<FLootTableData>("");
	}
	else
	{
		DropData = StandartDestructLootTable.GetRow<FLootTableData>("");
	}

	if (DropData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ULootTableComponent::DropLoot DropData == nullptr"));
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ULootTableComponent::DropLoot IsValid(World) == false"));
		return;
	}
	UMeshComponent* OwnerMesh = Cast<UMeshComponent>(GetOwner()->GetComponentByClass<UMeshComponent>());
	if (IsValid(OwnerMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ULootTableComponent::DropLoot IsValid(OwnerMesh) == false"));
		return;
	}

	FVector MinLocalBounds = OwnerMesh->GetLocalBounds().GetBox().Min;
	FVector MaxLocalBounds = OwnerMesh->GetLocalBounds().GetBox().Max;
	
	FVector ActorSize = MaxLocalBounds - MinLocalBounds;
	FVector ActorBottom = GetOwner()->GetActorLocation() + GetOwner()->GetActorUpVector() * MinLocalBounds.Z;
	FVector ActorTop = GetOwner()->GetActorLocation() + GetOwner()->GetActorUpVector() * MaxLocalBounds.Z;

	double LowerHeight = ActorSize.Z * LowerHeightPercent;
	double UpperHeight = ActorSize.Z * LowerHeightPercent;

	FVector CurrentSpawningPoint = ActorBottom;
	CurrentSpawningPoint += GetOwner()->GetActorUpVector() * (ActorSize.Z * LowerHeightPercent);

	int RNG = FMath::RandRange(1, 10000);

	for (TPair<TSubclassOf<AActor>, FLootTableEntryDropData> Drop : DropData->LootTableData)
	{
		if (Drop.Value.DropRate * 100 >= RNG)
		{
			int Amount = FMath::RandRange(Drop.Value.MinAmount, Drop.Value.MaxAmount);

			for (int i = 0; i < Amount; i++)
			{
				AActor* DroppedActor = World->SpawnActor<AActor>(Drop.Key, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation() + UKismetMathLibrary::RandomRotator() * RotationLimit);
				FVector NewActorLocation = GetOwner()->GetActorLocation();

				if (bShouldStackDropsVertically)
				{
					NewActorLocation = CurrentSpawningPoint;
					UStaticMeshComponent* DroppedActorMesh = DroppedActor->GetComponentByClass<UStaticMeshComponent>();
					if (IsValid(DroppedActorMesh) == false)
					{
						continue;
					}
					FVector NewActorMinBounds, NewActorMaxBounds;
					DroppedActorMesh->GetLocalBounds(NewActorMinBounds, NewActorMaxBounds);
					CurrentSpawningPoint += GetOwner()->GetActorUpVector() * (NewActorMaxBounds.Z - NewActorMinBounds.Z);
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
}


