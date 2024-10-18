// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/InGameOverlayWidget.h"

#include "Components/BackgroundBlur.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"

void UInGameOverlayWidget::UpdateHealthPercent(float CurrentHealthPercent)
{
	HealthBar->SetPercent(CurrentHealthPercent);

	UMaterialInstanceDynamic* HealthPercentOverlayMaterial = HealthPercentOverlay->GetDynamicMaterial();
	if (!HealthPercentOverlayMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("UInGameOverlayWidget::UpdateHealthPercent HealthPercentOverlayMaterial == nullptr"));
		return;
	}
	HealthPercentOverlayMaterial->SetScalarParameterValue("BoxDimensions", 0.2 + CurrentHealthPercent);
}

void UInGameOverlayWidget::UpdateHungerPercent(float CurrentHungerPercent)
{
	HungerBar->SetPercent(CurrentHungerPercent);
	HungerPercentOverlay->SetBrushColor(FLinearColor(0, 0, 0, 0.5 - CurrentHungerPercent));
}

void UInGameOverlayWidget::UpdateEnergyPercent(float CurrentEnergyPercent)
{
	EnergyBar->SetPercent(CurrentEnergyPercent);
	
	BackgroundBlur->SetBlurStrength(FMath::Clamp<float>(MaxBlurValue * (CurrentEnergyPercent * -2 + 1), 0, MaxBlurValue));
}
