// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Core/DamageIndicatorWidget.h"

#include "Components/TextBlock.h"

void UDamageIndicatorWidget::SetDamageText(float Damage)
{
	DamageText->SetText(FText::FromString(FString::SanitizeFloat(Damage)));
}
