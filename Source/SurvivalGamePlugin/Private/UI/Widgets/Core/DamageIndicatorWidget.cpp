// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Core/DamageIndicatorWidget.h"

#include "Components/TextBlock.h"

void UDamageIndicatorWidget::SetDamageText(float Damage)
{
	DamageText->SetText(FText::FromString(FString::SanitizeFloat(Damage)));

	double RedColorValue = -0.0000125f * Damage * Damage + 0.00125f * Damage + 1.f;
	double GreenColorValue = 0.000025f * Damage * Damage - 0.01f * Damage + 1.f;
	double BlueColorValue = 0.00005f * Damage * Damage - 0.015f * Damage + 1.f;

	UE_LOG(LogTemp, Log, TEXT("Damage Indicator Color - %.2f, %.2f, %.2f"), RedColorValue, GreenColorValue, BlueColorValue);

	DamageText->SetColorAndOpacity(FLinearColor(RedColorValue, GreenColorValue, BlueColorValue));
}
