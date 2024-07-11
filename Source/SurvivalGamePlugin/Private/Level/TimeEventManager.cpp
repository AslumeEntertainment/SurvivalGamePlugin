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
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"


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
}

// Called when the game starts or when spawned
void ATimeEventManager::BeginPlay()
{
	Super::BeginPlay();

	SetActorRotation(FRotator(InitialSkyRotation, 270, 0));
	SkyRotationComponent->RotationRate = FRotator(-360 / DayLength, 0, 0);
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
	if (!IsValid(TestActorClass))
	{
		UE_LOG(LogTemp, Error, TEXT("ATimeEventManager::SpawnPredator_Implementation IsValid(TestActorClass) == false"));
		return;
	}
	GetWorld()->SpawnActor<AActor>(TestActorClass, GetValidPredatorLocation(), FRotator(0, 0, 0));
}

FVector ATimeEventManager::GetValidPredatorLocation()
{
	FVector PredatorLocation;
	bool bIsValidLocation = true;

	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Players);

	TArray<FVector> PlayerLocations;
	for (AActor* Player : Players)
	{
		PlayerLocations.Add(Player->GetActorLocation());
	}

	do
	{
		PredatorLocation.X = FMath::RandRange(BottomLeftArenaCornerCoordinate.X, UpperRightArenaCornerCoordinate.X);
		PredatorLocation.Y = FMath::RandRange(BottomLeftArenaCornerCoordinate.Y, UpperRightArenaCornerCoordinate.Y);
		PredatorLocation.Z = FMath::RandRange(BottomLeftArenaCornerCoordinate.Z, UpperRightArenaCornerCoordinate.Z);

		/*
		for (FVector PlayerLocation : PlayerLocations)
		{
			if (UKismetMathLibrary::Vector_Distance(PlayerLocation, PredatorLocation) <= MinimalSpawnDistanceFromPlayer)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Invalid location (Distance = %.2f), searching for a new one..."), UKismetMathLibrary::Vector_Distance(PlayerLocation, PredatorLocation)));
				bIsValidLocation = false;
			}
		}
		*/
	} while (!bIsValidLocation);
	
	return PredatorLocation;
}

// Called every frame
void ATimeEventManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

