// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/UnitIconWidget.h"

#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UUnitIconWidget::OnIconClicked()
{
	UWrapBox* ParentWrapBox = Cast<UWrapBox>(this->GetParent());
	if (IsValid(ParentWrapBox) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemIconWidget::OnIconClicked IsValid(ParentWrapBox) == false"));
	}
	IconClicked.Broadcast(ParentWrapBox->GetChildIndex(this));
}

void UUnitIconWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Icon->OnClicked.AddDynamic(this, &UUnitIconWidget::OnIconClicked);
}

void UUnitIconWidget::SetItemName(FText ItemName)
{
	TXT_ItemName->SetText(ItemName);
}

void UUnitIconWidget::SetItemImage(UTexture2D* ItemImage)
{
	IMG_ItemIcon->SetBrushFromTexture(ItemImage);
}

void UUnitIconWidget::SetItemColor(FLinearColor Color)
{
	BTN_Icon->SetBackgroundColor(Color);
}

void UUnitIconWidget::SetUnitScale(float Scale)
{
	UnitSizeBox->SetWidthOverride(UnitSizeBox->GetWidthOverride() * Scale);
	UnitSizeBox->SetHeightOverride(UnitSizeBox->GetHeightOverride() * Scale);
}

void UUnitIconWidget::ConfigUnit(FText Name, UTexture2D* Image, FLinearColor Color, float Scale)
{
	TXT_ItemName->SetText(Name);
	IMG_ItemIcon->SetBrushFromTexture(Image);
	BTN_Icon->SetBackgroundColor(Color);
	SetUnitScale(Scale);
}
