// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/MapTile.h"

#include "Components/StaticMeshComponent.h"

AMapTile::AMapTile()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
}

void AMapTile::BeginPlay()
{
	Super::BeginPlay();
}

void AMapTile::ElevateTile(float HeightOffset)
{
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + HeightOffset));
}

void AMapTile::SetTileColor(FVector Color)
{
	UMaterialInstanceDynamic* DynamicMaterial = StaticMesh->CreateDynamicMaterialInstance(0);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("Color", Color);
	}
}


