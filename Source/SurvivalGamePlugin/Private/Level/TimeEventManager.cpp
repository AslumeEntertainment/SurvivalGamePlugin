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


// Sets default values
ATimeEventManager::ATimeEventManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SkySphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkySphere"));
	SetRootComponent(SkySphere);

	SunLightSource = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	SunLightSource->SetupAttachment(GetRootComponent());

	MoonLightSource = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("MoonLight"));
	MoonLightSource->SetupAttachment(GetRootComponent());

	ExponentialHeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("ExponentialHeightFog"));
	ExponentialHeightFog->SetupAttachment(GetRootComponent());

	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(GetRootComponent());

	VolumetricCloud = CreateDefaultSubobject<UVolumetricCloudComponent>(TEXT("VolumetricCloud"));
	VolumetricCloud->SetupAttachment(GetRootComponent());

	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(GetRootComponent());
}

void ATimeEventManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATimeEventManager, SkyRotation);
}

// Called when the game starts or when spawned
void ATimeEventManager::BeginPlay()
{
	Super::BeginPlay();

	BaseSkyRotationRate = 360 / DayLength;
	SkyRotation = InitialSkyRotation;
	if (bIsTimeFlowing)
	{
		//GetWorldTimerManager().SetTimer(DayNightCycleTimer, this, &ATimeEventManager::AdvanceTime, 0.01, true, 0);
	}
	GetWorldTimerManager().SetTimer(PreparationPeriodTimer, this, &ATimeEventManager::PreparationPeriodOver, DayLength*DaysForPreparation, 0);
}

void ATimeEventManager::AdvanceTime_Implementation()
{
	//SunLightSource->SetRelativeRotation(FRotator(InitialSkyRotation + 0.01 * AdditionalSkyRotation, 0, 0));
	//MoonLightSource->SetRelativeRotation(FRotator((InitialSkyRotation + 180) + 0.01 * AdditionalSkyRotation, 0, 0));
	//SkySphere->SetRelativeRotation(FRotator(InitialSkyRotation + 0.01 * AdditionalSkyRotation, 0, 0));
	//AdditionalSkyRotation += BaseSkyRotationRate;
}

void ATimeEventManager::PreparationPeriodOver_Implementation()
{
	UGameplayStatics::PlaySound2D(GetWorld(), NotificationSound, 1);
}

// Called every frame
void ATimeEventManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SkyRotation += DeltaTime * BaseSkyRotationRate;
	SunLightSource->SetRelativeRotation(FRotator(SkyRotation, 0, 0));
	MoonLightSource->SetRelativeRotation(FRotator(SkyRotation + 180, 0, 0));
	SkySphere->SetRelativeRotation(FRotator(SkyRotation, 0, 0));
}

