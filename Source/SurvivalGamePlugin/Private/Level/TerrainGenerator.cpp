// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/TerrainGenerator.h"

#include "Kismet/KismetSystemLibrary.h"


ATerrainGenerator::ATerrainGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATerrainGenerator::BeginPlay()
{
	Super::BeginPlay();

	Center = GetActorLocation();

	SquareMethod_GenerateTerrainCoordinates();
	//Bresenham_GenerateTerrainCoordinates();
}

void ATerrainGenerator::SquareMethod_GenerateTerrainCoordinates()
{
	FVector StartPoint = FVector(Center.X - Radius * TileSize, Center.Y - Radius * TileSize, Center.Z);
	
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ATerrainGenerator::SquareMethod_GenerateTerrainCoordinates IsValid(World) == false"));
		return;
	}

	for (int i = StartPoint.X; i <= Center.X + Radius * TileSize; i += TileSize)
	{
		for (int j = StartPoint.Y; j <= Center.Y + Radius * TileSize; j += TileSize)
		{
			AActor* Tile = World->SpawnActor<AActor>(TileClass, FVector(i, j, Center.Z), GetActorRotation());
			if (IsValid(Tile) == false)
			{
				continue;
			}
			Tiles.Add(Tile);
		}
	}

	//1 - sphere trace r-1 - these are safe fields
	//2 - sphere trace r and ignore 1 - border tiles
	//3 - sphere trace 2*r (squrt(2)*r) and ignore 1 and 2 - tiles to delete

	//actual generation (try to randomly choose 1 or 2 special tiles)

	/*TArray<FHitResult> HitResults;

	if (UKismetSystemLibrary::SphereOverlapActors(World, Center, Radius * TileSize, UEngineTypes::ConvertToTraceType(ECC_Camera),
		true, ActorsToIgnoreWhenActive, EDrawDebugTrace::ForOneFrame, HitResults, true) == false)
	{
		return;
	}*/

}

void ATerrainGenerator::Bresenham_GenerateTerrainCoordinates()
{
	int x = 0;
	int y = Radius * TileSize;
	//int d = 3 - 2 * Radius; //GEEKED
	int d = (5 - 4 * Radius) / 4 * TileSize; //LOCKED IN
	//int d = 1 - Radius; //INDIAN

	AddCoordinates(x, y);

	while (y > x)
	{
		//GEEKED
		/*if (d > 0) 
		{
			y -= TileSize;
			d = d + 4 * (x - y) + 10;
		}
		else
		{
			d = d + 4 * x + 6;
		}*/

		//LOCKED IN
		if (d < 0)
		{
			d += 2 * x + 1;
		}
		else
		{
			y -= TileSize;
			d += 2 * (x - y) + 1;
		}

		x += TileSize;

		AddCoordinates(x, y);
	}

	PlaceTiles();
}

void ATerrainGenerator::AddCoordinates(int x, int y)
{
	Coordinates.Add(FVector(Center.X+x, Center.Y+y, Center.Z));
	Coordinates.Add(FVector(Center.X+x, Center.Y-y, Center.Z));
	
	Coordinates.Add(FVector(Center.X-x, Center.Y+y, Center.Z));
	Coordinates.Add(FVector(Center.X-x, Center.Y-y, Center.Z));

	Coordinates.Add(FVector(Center.X+y, Center.Y+x, Center.Z));
	Coordinates.Add(FVector(Center.X+y, Center.Y-x, Center.Z));

	Coordinates.Add(FVector(Center.X-y, Center.Y+x, Center.Z));
	Coordinates.Add(FVector(Center.X-y, Center.Y-x, Center.Z));

	FillInnerCoordinates(x, y);
}

void ATerrainGenerator::FillInnerCoordinates(int x, int y)
{
	for (float i = y - TileSize; i >= 0; i -= TileSize)
	{
		Coordinates.Add(FVector(Center.X + x, Center.Y + i, Center.Z));
		Coordinates.Add(FVector(Center.X - x, Center.Y + i, Center.Z));

		Coordinates.Add(FVector(Center.X + x, Center.Y - i, Center.Z));
		Coordinates.Add(FVector(Center.X - x, Center.Y - i, Center.Z));

		Coordinates.Add(FVector(Center.X + i, Center.Y + x, Center.Z));
		Coordinates.Add(FVector(Center.X - i, Center.Y + x, Center.Z));

		Coordinates.Add(FVector(Center.X + i, Center.Y - x, Center.Z));
		Coordinates.Add(FVector(Center.X - i, Center.Y - x, Center.Z));
	}
}

void ATerrainGenerator::PlaceTiles()
{
	if (HasAuthority() == false)
	{
		//return;
	}
	if (IsValid(TileClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ATerrainGenerator::PlaceTiles IsValid(TileClass) == false"));
		return;
	}
	if (IsValid(BorderClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ATerrainGenerator::PlaceTiles IsValid(BorderClass) == false"));
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ATerrainGenerator::PlaceTiles IsValid(World) == false"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Number of tiles to generate - %d"), Coordinates.Num());
	for (const FVector Coordinate : Coordinates)
	{
		World->SpawnActor<AActor>(TileClass, Coordinate, GetActorRotation());
	}
	
	AActor* Border = World->SpawnActor<AActor>(BorderClass, Center, GetActorRotation());
	Border->SetActorScale3D(FVector(Radius * 2 + 1, Radius * 2 + 1, 1));
}




