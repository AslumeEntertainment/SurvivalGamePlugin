// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/ShopUnitWidgetBase.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UShopUnitWidgetBase::NativeOnInitialized()
{

	Super::NativeOnInitialized();
	BTN_Icon->OnClicked.AddDynamic(this, &UShopUnitWidgetBase::OnBodyButtonClicked);
}

void UShopUnitWidgetBase::Config(FText Name, UTexture2D* Image, FLinearColor Color, uint16 Cost)
{
	BTN_Icon->SetBackgroundColor(Color);
	TXT_Name->SetText(Name);
	TXT_Cost->SetText(FText::FromString(Cost == InvisibleCost ? "" : FString::FromInt(Cost)));
	IMG_Icon->SetBrushFromTexture(Image);
}

void UShopUnitWidgetBase::OnBodyButtonClicked()
{
	OnBodyButton.Broadcast(this);
}

