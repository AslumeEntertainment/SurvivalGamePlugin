// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/ItemIconWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemIconWidget::OnIconClicked()
{
	FString TextString = TXT_ItemName->GetText().ToString();
	TextString.RemoveSpacesInline();
	IconClicked.Broadcast(FName(*TextString));
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
