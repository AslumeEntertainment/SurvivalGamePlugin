// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapTile.generated.h"

class StaticMesh;

UCLASS()
class SURVIVALGAMEPLUGIN_API AMapTile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AMapTile();

protected:
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	virtual void BeginPlay() override;

public:

	void ElevateTile(float HeightOffset);

	void SetTileColor(FVector Color);

};
