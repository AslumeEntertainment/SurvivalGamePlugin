// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameOverlayWidget.generated.h"

class UBorder;
class UTextBlock;
class UProgressBar;
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
	UTextBlock* TXT_CurrentSpeed;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* HungerBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* EnergyBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;

	UPROPERTY(EditAnywhere)
	int MaxBlurValue = 15;

public:

	UFUNCTION()
	void UpdateCurrentSpeed(float NewValue);

	UFUNCTION()
	void UpdateHealthPercent(float CurrentHealthPercent);

	UFUNCTION()
	void UpdateHungerPercent(float CurrentHungerPercent);

	UFUNCTION()
	void UpdateEnergyPercent(float CurrentEnergyPercent);

	UFUNCTION()
	void SetHealthBarLength(float PlayerMaxHealth);

	UFUNCTION()
	void SetHungerBarLength(float PlayerMaxHunger);

	UFUNCTION()
	void SetEnergyBarLength(float PlayerMaxEnergy);

};
