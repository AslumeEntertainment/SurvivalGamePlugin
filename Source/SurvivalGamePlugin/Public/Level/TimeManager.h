// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeManager.generated.h"

UCLASS()
class SURVIVALGAMEPLUGIN_API ATimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimeManager();

private:

	float BaseSkyRotationRate;

	UPROPERTY(Replicated)
	float AdditionalSkyRotation = 0;

protected:

	virtual void BeginPlay() override;

	FTimerHandle DayNightCycleTimer;
	FTimerHandle PreparationPeriodTimer;
	FTimerHandle SurvivalPeriodTimer;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	int DaysForPreparation = 1;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	float DayLength = 600;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	float InitialSkyRotation = -60;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	AActor* Sun;

	UPROPERTY(EditAnywhere, Category = "DayNightCycle")
	class ADirectionalLight* LightSource;

	UPROPERTY(EditAnywhere, Category = "PreparationTimeOver")
	USoundBase* NotificationSound;

	UFUNCTION(/*Client, Reliable*/)
	void AdvanceTime();

	UFUNCTION(Server, Reliable)
	void PreparationPeriodOver();
	
public:	

	virtual void Tick(float DeltaTime) override;

};
