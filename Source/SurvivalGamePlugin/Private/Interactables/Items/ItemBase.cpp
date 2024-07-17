// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/ItemBase.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemBase::Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Using item"));
}

void AItemBase::InteractRequest_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Picking up item"));
}

