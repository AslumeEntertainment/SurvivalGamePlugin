// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/InGameOverlayWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/BackgroundBlur.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"


void UInGameOverlayWidget::UpdateCurrentSpeed(float NewValue)
{
	TXT_CurrentSpeed->SetText(FText::FromString(FString::SanitizeFloat(FMath::CeilToFloat(NewValue / 100)) + " m/s"));
}

void UInGameOverlayWidget::UpdateHealthPercent(float CurrentHealthPercent)
{
	UMaterialInstanceDynamic* HealthPercentOverlayMaterial = IMG_HealthPercentOverlay->GetDynamicMaterial();
	if (HealthPercentOverlayMaterial)
	{
		HealthPercentOverlayMaterial->SetScalarParameterValue("BoxDimensions", 0.2 + CurrentHealthPercent);
	}
	
	HealthBar->SetPercent(CurrentHealthPercent);
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

void UInGameOverlayWidget::SetHealthBarLength(float PlayerMaxHealth)
{
	UCanvasPanelSlot* HealthBarAsSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(HealthBar);
	if (IsValid(HealthBarAsSlot) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UInGameOverlayWidget::SetHealthBarLength IsValid(HealthBarAsSlot) == false"));
		return;
	}
	HealthBarAsSlot->SetSize(FVector2D(PlayerMaxHealth * 5, 25));
}

void UInGameOverlayWidget::SetHungerBarLength(float PlayerMaxHunger)
{
	UCanvasPanelSlot* HungerBarAsSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(HungerBar);
	if (IsValid(HungerBarAsSlot) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UInGameOverlayWidget::SetHungerBarLength IsValid(HungerBarAsSlot) == false"));
		return;
	}
	HungerBarAsSlot->SetSize(FVector2D(PlayerMaxHunger * 5, 25));
}

void UInGameOverlayWidget::SetEnergyBarLength(float PlayerMaxEnergy)
{
	UCanvasPanelSlot* EnergyBarAsSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(EnergyBar);
	if (IsValid(EnergyBarAsSlot) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UInGameOverlayWidget::SetEnergyBarLength IsValid(EnergyBarAsSlot) == false"));
		return;
	}
	EnergyBarAsSlot->SetSize(FVector2D(PlayerMaxEnergy * 5, 25));
}
