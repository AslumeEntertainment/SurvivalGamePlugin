// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeEventManager.generated.h"

UCLASS()
class SURVIVALGAMEPLUGIN_API ATimeEventManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimeEventManager();

private:

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	TSubclassOf<AActor> TestActorClass;

protected:

	virtual void BeginPlay() override;

	FTimerHandle PreparationPeriodTimer;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	float DayLength = 30;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	float InitialSkyRotation = 90;

	UPROPERTY(EditAnywhere, Category = "PredatorSpawning")
	int DaysForPreparation = 1;

	UPROPERTY(EditAnywhere, Category = "PredatorSpawning")
	USoundBase* NotificationSound;

	UPROPERTY(EditAnywhere, Category = "PredatorSpawning")
	FVector BottomLeftArenaCornerCoordinate;

	UPROPERTY(EditAnywhere, Category = "PredatorSpawning")
	FVector UpperRightArenaCornerCoordinate;

	UPROPERTY(EditAnywhere, Category = "PredatorSpawning")
	float MinimalSpawnDistanceFromPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class UStaticMeshComponent* SkySphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class URotatingMovementComponent* SkyRotationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class UDirectionalLightComponent* SunLightSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class UDirectionalLightComponent* MoonLightSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class UExponentialHeightFogComponent* ExponentialHeightFog;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class USkyLightComponent* SkyLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class UVolumetricCloudComponent* VolumetricCloud;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class USkyAtmosphereComponent* SkyAtmosphere;;

	void StartPreparationPeriod();

	UFUNCTION(NetMulticast, Reliable)
	void PreparationPeriodOver();

	void SpawnPredator();

	FVector GetValidPredatorLocation();
	
public:	

	virtual void Tick(float DeltaTime) override;

};
