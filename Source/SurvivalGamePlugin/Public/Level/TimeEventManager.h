// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeEventManager.generated.h"

DECLARE_DYNAMIC_DELEGATE(FPreparationPeriodOverSignature);

UCLASS()
class SURVIVALGAMEPLUGIN_API ATimeEventManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimeEventManager();

protected:

	FTimerHandle PreparationPeriodTimer;

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

	void StartPreparationPeriod(float InDaysForPreparation);
	void PreparationPeriodOver();

	
public:	

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldPrintDebug = true;

	FPreparationPeriodOverSignature OnPreparationOver;
};
