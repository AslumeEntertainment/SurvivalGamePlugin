// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/ActorSpawner.h"

// Sets default values
AActorSpawner::AActorSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorSpawner::Spawn()
{
	int RNG = FMath::RandRange(1, 10000);

	for (TPair<TSubclassOf<AActor>, float > ActorToSpawn : SpawnableActors) 
	{
		if (ActorToSpawn.Value*100 >= RNG)
		{
			GetWorld()->SpawnActor<AActor>(ActorToSpawn.Key, GetActorLocation(), GetActorRotation());
			return;
		}
	}

}

