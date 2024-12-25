// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/UnitIconWidget.h"

#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UUnitIconWidget::OnIconClicked()
{
	UPanelWidget* ParentWidget = Cast<UPanelWidget>(this->GetParent());
	if (IsValid(ParentWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemIconWidget::OnIconClicked IsValid(ParentWidget) == false"));
		return;
	}
	IconClicked.Broadcast(ParentWidget->GetChildIndex(this));
}

void UUnitIconWidget::BroadcastParentOnIconClicked()
{
	UPanelWidget* ParentWidget = Cast<UPanelWidget>(this->GetParent());
	if (IsValid(ParentWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemIconWidget::BroadcastParentOnIconClicked IsValid(ParentWidget) == false"));
		return;
	}
	BroadcastParentOnClicked.Broadcast(ParentWidget);
}

void UUnitIconWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Icon->OnClicked.AddDynamic(this, &UUnitIconWidget::OnIconClicked);
	BTN_Icon->OnClicked.AddDynamic(this, &UUnitIconWidget::BroadcastParentOnIconClicked);
}

void UUnitIconWidget::SetItemName(FText ItemName)
{
	TXT_ItemName->SetText(ItemName);
}

FString UUnitIconWidget::GetItemName()
{
	return TXT_ItemName->GetText().ToString();
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
