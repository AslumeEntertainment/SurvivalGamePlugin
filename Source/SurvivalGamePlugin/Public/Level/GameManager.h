// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class APlayerCustomiser;

UCLASS()
class SURVIVALGAMEPLUGIN_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "PlayerCustomiser")
	TSubclassOf<APlayerCustomiser> PlayerCustomiserClassToSpawn;

	UPROPERTY(EditAnywhere, Category = "PlayerCustomiser")
	FName PlayerCustomiserSpawnComponentTag = FName("Chair");

	UPROPERTY(VisibleDefaultsOnly)
	TMap<APlayerCustomiser*, int> Teams;

public:	

	void Open();
	void Close();
};
