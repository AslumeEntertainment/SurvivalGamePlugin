// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelCleaner.generated.h"

UCLASS()
class SURVIVALGAMEPLUGIN_API ALevelCleaner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelCleaner();

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionMesh = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FName TagToPersist;

	UPROPERTY(EditAnywhere)
	FName TagToClean;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Clean();

};
