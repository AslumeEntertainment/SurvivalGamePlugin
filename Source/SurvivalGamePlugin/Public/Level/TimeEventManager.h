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
	
	float BaseSkyRotationRate;

	UPROPERTY(Replicated)
	float SkyRotation;

protected:

	virtual void BeginPlay() override;

	FTimerHandle DayNightCycleTimer;
	FTimerHandle PreparationPeriodTimer;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	float DayLength = 600;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	float InitialSkyRotation = -60;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	bool bIsTimeFlowing = true;

	UPROPERTY(EditAnywhere, Category = "PredatorSpawning")
	int DaysForPreparation = 1;

	UPROPERTY(EditAnywhere, Category = "PredatorSpawning")
	USoundBase* NotificationSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Sky")
	class UStaticMeshComponent* SkySphere;

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

	UFUNCTION(NetMulticast, Reliable)
	void AdvanceTime();

	UFUNCTION(Server, Reliable)
	void PreparationPeriodOver();
	
public:	

	virtual void Tick(float DeltaTime) override;

};
