// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/Shop/ShopUnitWidgetBase.h"

#include "Components/TextBlock.h"
#include "Components/ScaleBox.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UShopUnitWidgetBase::SetUnitCost(uint16 Cost)
{
	TXT_Cost->SetText(FText::FromString(Cost == InvisibleCost ? "" : FString::FromInt(Cost)));
}

void UShopUnitWidgetBase::ConfigShopUnit(FText Name, UTexture2D* Image, FLinearColor Color, uint16 Cost, float Scale)
{
	ConfigUnit(Name, Image, Color, Scale);
	TXT_Cost->SetText(FText::FromString(Cost == InvisibleCost ? "" : FString::FromInt(Cost)));
}


