// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/TimeManager.h"
#include "Engine/DirectionalLight.h"
#include "Misc/OutputDeviceNull.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATimeManager::ATimeManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATimeManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATimeManager, AdditionalSkyRotation);
}

// Called when the game starts or when spawned
void ATimeManager::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(LightSource))
	{
		UE_LOG(LogTemp, Error, TEXT("ATimeManager::BeginPlay IsValid(LightSource) == false"));
		return;
	}
	BaseSkyRotationRate = 360 / DayLength;
	//StartingSkyRotation = LightSource->GetActorRotation().Pitch;

	GetWorldTimerManager().SetTimer(DayNightCycleTimer, this, &ATimeManager::AdvanceTime, 0.01, true, 0);
	GetWorldTimerManager().SetTimer(PreparationPeriodTimer, this, &ATimeManager::PreparationPeriodOver, DayLength*DaysForPreparation, 0);
}

void ATimeManager::AdvanceTime/*_Implementation*/()
{
	if (!IsValid(LightSource))
	{
		UE_LOG(LogTemp, Error, TEXT("ATimeManager::TimeAdvance IsValid(LightSource) == false"));
		return;
	}
	if (!IsValid(Sun))
	{
		UE_LOG(LogTemp, Error, TEXT("ATimeManager::TimeAdvance IsValid(Sun) == false"));
		return;
	}

	LightSource->SetActorRotation(FRotator(InitialSkyRotation + 0.01 * AdditionalSkyRotation, 0, 0));
	AdditionalSkyRotation += BaseSkyRotationRate;
	FOutputDeviceNull OutputDeviceNull;
	Sun->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), OutputDeviceNull, NULL, true);
}

void ATimeManager::PreparationPeriodOver_Implementation()
{
	UGameplayStatics::PlaySound2D(GetWorld(), NotificationSound, 1);
}

// Called every frame
void ATimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

