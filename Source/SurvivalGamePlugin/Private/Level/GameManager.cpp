// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/GameManager.h"

#include "Level/PlayerCustomiser.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT(" AGameManager::BeginPlay IsValid(World) == false"));
		return;
	}

	TArray<UActorComponent*> ChairSpawns = GetComponentsByTag(USceneComponent::StaticClass(), PlayerCustomiserSpawnComponentTag);
	for (UActorComponent* ChairSpawnPtr : ChairSpawns)
	{
		USceneComponent* ChairSpawn = Cast<USceneComponent>(ChairSpawnPtr);
		if (IsValid(ChairSpawn) == false) continue;

		FVector Location = ChairSpawn->GetComponentLocation();
		FRotator Rotation = ChairSpawn->GetComponentRotation();
		FActorSpawnParameters SpawnInfo;
		APlayerCustomiser* PlayerCustomiser = World->SpawnActor<APlayerCustomiser>(PlayerCustomiserClassToSpawn, Location, Rotation, SpawnInfo);

		Teams.Add(PlayerCustomiser, 0);
	}
}

void AGameManager::Open()
{
	//make the chairs active
}

void AGameManager::Close()
{
	//make the chairs not active
}


