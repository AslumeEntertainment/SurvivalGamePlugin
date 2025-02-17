// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/TerrainGenerator.h"

#include "Kismet/KismetSystemLibrary.h"

#include "Level/MapTile.h"

ATerrainGenerator::ATerrainGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATerrainGenerator::BeginPlay()
{
	Super::BeginPlay();

	Center = GetActorLocation();

	SquareMethod_GenerateTerrain();
	DetermineSpecialTiles(3);

	//Bresenham_GenerateTerrainCoordinates();
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
	
	PlaceBorder(World);
}

void ATerrainGenerator::PlaceBorder(UWorld* World)
{
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ATerrainGenerator::PlaceBorder IsValid(World) == false"));
		return;
	}
	AActor* Border = World->SpawnActor<AActor>(BorderClass, Center, GetActorRotation());
	Border->SetActorScale3D(FVector(Radius * 2 + 1, Radius * 2 + 1, 1));
}

void ATerrainGenerator::SquareMethod_GenerateTerrain()
{
	FVector StartPoint = FVector(Center.X - Radius * TileSize, Center.Y - Radius * TileSize, Center.Z);

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ATerrainGenerator::SquareMethod_GenerateTerrain IsValid(World) == false"));
		return;
	}

	for (int i = StartPoint.X; i <= Center.X + Radius * TileSize; i += TileSize)
	{
		for (int j = StartPoint.Y; j <= Center.Y + Radius * TileSize; j += TileSize)
		{
			AMapTile* Tile = World->SpawnActor<AMapTile>(TileClass, FVector(i, j, Center.Z), GetActorRotation());
			if (IsValid(Tile) == false)
			{
				continue;
			}
			Tiles.Add(Tile);
		}
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	TArray<AActor*> TilesToIgnore;

	UKismetSystemLibrary::SphereOverlapActors(World, Center, (Radius - 1) * TileSize + TileSize / 4, ObjectTypes, AMapTile::StaticClass(), TilesToIgnore, TilesToIgnore);

	TArray<AActor*> Borders;

	UKismetSystemLibrary::SphereOverlapActors(World, Center, Radius * TileSize + TileSize / 2, ObjectTypes, AMapTile::StaticClass(), TilesToIgnore, Borders);
	
	for (AActor* Tile : Borders)
	{
		AMapTile* MapTile = Cast<AMapTile>(Tile);
		if (IsValid(MapTile) == false)
		{
			continue;
		}
		TilesToIgnore.Add(Tile);
		BorderTiles.Add(MapTile);

		MapTile->SetTileColor(FVector(1, 0, 0));
	}

	TArray<AActor*> TilesToDestroy;

	UKismetSystemLibrary::SphereOverlapActors(World, Center, Radius * 1.414f * TileSize, ObjectTypes, AMapTile::StaticClass(), TilesToIgnore, TilesToDestroy);

	for (AActor* Tile : TilesToDestroy)
	{
		AMapTile* MapTile = Cast<AMapTile>(Tile);
		if (IsValid(MapTile) && Tiles.Contains(MapTile))
		{
			Tiles.Remove(MapTile);
		}
		Tile->Destroy();
	}

	UE_LOG(LogTemp, Warning, TEXT("%s: Tiles in total - %d"), *GetName(), Tiles.Num());
	UE_LOG(LogTemp, Warning, TEXT("%s: Border tiles - %d"), *GetName(), BorderTiles.Num());

	PlaceBorder(World);
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

TArray<AMapTile*> ATerrainGenerator::DetermineSpecialTiles(int TileCount)
{
	TArray<AMapTile*> SpecialTiles;
	TArray<int> Indexes;

	for (int i = 0; i < TileCount; i++)
	{
		int RandomIndex;
		do
		{
			RandomIndex = FMath::RandRange(0, Tiles.Num() - 1);
		} while (BorderTiles.Contains(Tiles[RandomIndex]) || Indexes.Contains(RandomIndex));

		SpecialTiles.Add(Tiles[RandomIndex]);
		Tiles[RandomIndex]->SetTileColor(FVector(0, 0.5f, 0.5f));
	}

	return SpecialTiles;
}


