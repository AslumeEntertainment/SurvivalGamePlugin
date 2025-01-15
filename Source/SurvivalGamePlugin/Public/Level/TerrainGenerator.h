// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainGenerator.generated.h"

UCLASS()
class SURVIVALGAMEPLUGIN_API ATerrainGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ATerrainGenerator();

private:

	FVector Center;

	UPROPERTY()
	TArray<AActor*> Tiles;

	UPROPERTY()
	TSet<FVector> Coordinates;

protected:
	
	UPROPERTY(EditAnywhere, Category = "TerrainGeneration")
	int Radius = 1;

	UPROPERTY(EditAnywhere, Category = "TerrainGeneration")
	TSubclassOf<AActor> TileClass;

	UPROPERTY(EditAnywhere, Category = "TerrainGeneration")
	TSubclassOf<AActor> BorderClass;

	UPROPERTY(EditAnywhere, Category = "TerrainGeneration")
	int TileSize = 100;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SquareMethod_GenerateTerrainCoordinates();

	UFUNCTION(BlueprintCallable)
	void Bresenham_GenerateTerrainCoordinates();

	UFUNCTION(BlueprintCallable)
	void AddCoordinates(int x, int y);

	UFUNCTION(BlueprintCallable)
	void FillInnerCoordinates(int x, int y);

	UFUNCTION(BlueprintCallable)
	void PlaceTiles();


};
