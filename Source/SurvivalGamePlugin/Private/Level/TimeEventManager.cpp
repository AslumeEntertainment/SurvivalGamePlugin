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
	PrimaryActorTick.bCanEverTick = true;

	SkySphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkySphere"));
	SetRootComponent(SkySphere);

	SkyRotationComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));

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

	SetActorRotation(FRotator(InitialSkyRotation, 0, 0));
	SkyRotationComponent->RotationRate = FRotator(-360/DayLength, 0, 0);
}

// Called when the game starts or when spawned
void ATimeEventManager::BeginPlay()
{
	Super::BeginPlay();

	StartPreparationPeriod();
}

void ATimeEventManager::StartPreparationPeriod_Implementation()
{
	GetWorldTimerManager().SetTimer(PreparationPeriodTimer, this, &ATimeEventManager::PreparationPeriodOver, DayLength * DaysForPreparation, 0);
}

void ATimeEventManager::PreparationPeriodOver_Implementation()
{
	UGameplayStatics::PlaySound2D(GetWorld(), NotificationSound, 1);
	SpawnPredator();
}

void ATimeEventManager::SpawnPredator_Implementation()
{
	if (IsValid(TestActorClass))
	{
		FVector Location;
		Location.X = FMath::RandRange(BottomLeftArenaCornerCoordinate.X, UpperRightArenaCornerCoordinate.X);
		Location.Y = FMath::RandRange(BottomLeftArenaCornerCoordinate.Y, UpperRightArenaCornerCoordinate.Y);
		Location.Z = FMath::RandRange(BottomLeftArenaCornerCoordinate.Z, UpperRightArenaCornerCoordinate.Z);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		GetWorld()->SpawnActor<AActor>(TestActorClass, Location, Rotation, SpawnInfo);
	}
}

// Called every frame
void ATimeEventManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

