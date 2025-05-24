// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSpawner.generated.h"

UCLASS()
class SURVIVALGAMEPLUGIN_API AActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AActorSpawner();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomValues")
	TMap<TSubclassOf<AActor>, float> SpawnableActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomValues")
	FVector LocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomValues")
	FRotator RotationOffset;


public:	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomValues")
	bool bSpawnStatic = false;

	UFUNCTION(BlueprintImplementableEvent)
	void PostSpawn(AActor* SpawnedActor);

	AActor* Spawn();

};
