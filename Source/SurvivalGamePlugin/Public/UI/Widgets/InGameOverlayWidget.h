// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameOverlayWidget.generated.h"

class UBorder;
class UBackgroundBlur;

UCLASS()
class SURVIVALGAMEPLUGIN_API UInGameOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UBorder* HealthPercentOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UBorder* HungerPercentOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;

	UPROPERTY(EditAnywhere)
	int MaxBlurValue = 15;

public:

	UFUNCTION()
	void UpdateHealthPercent(float CurrentHealthPercent);

	UFUNCTION()
	void UpdateHungerPercent(float CurrentHungerPercent);

	UFUNCTION()
	void UpdateEnergyPercent(float CurrentEneryPercent);

};
