// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/ActorSpawner.h"

#include "Actors/SG_HittableActor.h"

// Sets default values
AActorSpawner::AActorSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

AActor* AActorSpawner::Spawn()
{
	int RNG = FMath::RandRange(1, 10000);

	for (TPair<TSubclassOf<AActor>, float > ActorToSpawn : SpawnableActors) 
	{
		if (ActorToSpawn.Value*100 >= RNG)
		{
			AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorToSpawn.Key, GetActorLocation(), GetActorRotation());
			if (IsValid(Actor) == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("AActorSpawner::Spawn - %s failed to spawn %s"), *GetName(), *ActorToSpawn.Key->StaticClass()->GetName());
				return nullptr;
			}

			PostSpawn(Actor);

			if (bSpawnStatic && Cast<ASG_HittableActor>(Actor))
			{
				Cast<ASG_HittableActor>(Actor)->Server_SetActorStatic();
			}

			return Actor;
		}
	}
	return nullptr;
}

