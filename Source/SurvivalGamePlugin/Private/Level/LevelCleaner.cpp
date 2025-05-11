// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LevelCleaner.h"

#include "Components/BoxComponent.h"

// Sets default values
ALevelCleaner::ALevelCleaner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("Collision Mesh"));
	SetRootComponent(CollisionMesh);
}

// Called when the game starts or when spawned
void ALevelCleaner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelCleaner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelCleaner::Clean()
{
	TArray<AActor*> ActorsToClean;
	CollisionMesh->GetOverlappingActors(ActorsToClean);
	for (AActor* Actor : ActorsToClean)
	{
		if (Actor->ActorHasTag(TagToPersist)) continue;
		Actor->Destroy();
		/*if (Actor->ActorHasTag(TagToClean))
		{
			Actor->Destroy();
		}*/
	}
}

