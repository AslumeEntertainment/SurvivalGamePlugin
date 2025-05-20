// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/TimeEventManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"



// Sets default values
ATimeEventManager::ATimeEventManager()
{
	PrimaryActorTick.bCanEverTick = false; //if not working set to true

	SkySphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkySphere"));
	SetRootComponent(SkySphere);

	SkyRotationComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));

	SunLightSource = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	SunLightSource->SetupAttachment(GetRootComponent());

	MoonLightSource = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("MoonLight"));
	MoonLightSource->SetupAttachment(GetRootComponent());

	ExponentialHeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("ExponentialHeightFog"));
	ExponentialHeightFog->SetupAttachment(GetRootComponent());

	VolumetricCloud = CreateDefaultSubobject<UVolumetricCloudComponent>(TEXT("VolumetricCloud"));
	VolumetricCloud->SetupAttachment(GetRootComponent());

	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(GetRootComponent());
}


void ATimeEventManager::StartSkyRotation()
{
	SetActorRotation(FRotator(FMath::RandRange(0, 360), 270, 0));
	SkyRotationComponent->RotationRate = FRotator(-360 / DayLength, 0, 0);
}

void ATimeEventManager::StopSkyRotation()
{
	SkyRotationComponent->RotationRate = FRotator(0, 0, 0);
}


