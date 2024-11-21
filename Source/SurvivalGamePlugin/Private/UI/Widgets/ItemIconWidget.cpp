// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/ItemIconWidget.h"

#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UItemIconWidget::OnIconClicked()
{
	UWrapBox* ParentWrapBox = Cast<UWrapBox>(this->GetParent());
	if (IsValid(ParentWrapBox) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemIconWidget::OnIconClicked IsValid(ParentWrapBox) == false"));
	}
	IconClicked.Broadcast(ParentWrapBox->GetChildIndex(this));
}

void UItemIconWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Icon->OnClicked.AddDynamic(this, &UItemIconWidget::OnIconClicked);
}

void UItemIconWidget::SetItemName(FText ItemName)
{
	TXT_ItemName->SetText(ItemName);
}

void UItemIconWidget::SetItemImage(UTexture2D* ItemImage)
{
	IMG_ItemIcon->SetBrushFromTexture(ItemImage);
}

void UItemIconWidget::SetItemColor(FLinearColor Color)
{
	BTN_Icon->SetBackgroundColor(Color);
}
