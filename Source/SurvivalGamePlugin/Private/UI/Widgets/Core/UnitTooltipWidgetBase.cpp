// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Core/UnitTooltipWidgetBase.h"

#include "Components/TextBlock.h"

void UUnitTooltipWidgetBase::SetUnitName(FString UnitName)
{
	TXT_UnitName->SetText(FText::FromString(UnitName));
}

void UUnitTooltipWidgetBase::SetUnitDescription(FString UnitDescription)
{
	TXT_UnitDescription->SetText(FText::FromString(UnitDescription));
}

