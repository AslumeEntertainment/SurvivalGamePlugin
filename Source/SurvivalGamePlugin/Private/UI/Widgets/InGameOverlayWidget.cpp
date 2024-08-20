// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/InGameOverlayWidget.h"
#include "Components/Border.h"

void UInGameOverlayWidget::UpdateHealthPercent(float CurrentHealthPercent)
{
	UE_LOG(LogTemp, Warning, TEXT("UInGameOverlayWidget::UpdateHealthPercent Player's HealthPercent = %.2f"), CurrentHealthPercent);
	UMaterialInstanceDynamic* HealthPercentOverlayMaterial = HealthPercentOverlay->GetDynamicMaterial();
	if (!HealthPercentOverlayMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("UInGameOverlayWidget::UpdateHealthPercent HealthPercentOverlayMaterial == nullptr"));
		return;
	}
	HealthPercentOverlayMaterial->SetScalarParameterValue("BoxDimensions", 0.2 + CurrentHealthPercent);
}
