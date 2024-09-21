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

protected:

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	float DayLength = 30;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	float InitialSkyRotation = 90;

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

public:	

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldPrintDebug = true;

	UFUNCTION()
	void StartSkyRotation();

	UFUNCTION()
	void StopSkyRotation();

};
