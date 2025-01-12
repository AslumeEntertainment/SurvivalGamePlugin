// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/UnitIconWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"

#include "UI/Widgets/Core/CardSlotWidget.h"
#include "UI/DragWidget.h"


FEventReply UUnitIconWidget::RedirectMouseDownToWidget(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return Reply;
}

FReply UUnitIconWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return RedirectMouseDownToWidget(InGeometry, InMouseEvent).NativeReply;
}

FReply UUnitIconWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	FEventReply ReplyResult;

	if (bCanBeDragged)
	{
		ReplyResult = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	}
	
	return ReplyResult.NativeReply;
}

void UUnitIconWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (bCanBeDragged == false)
	{
		return;
	}

	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (IsValid(GetParent()) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UUnitIconWidget::NativeOnDragDetected"));
		return;
	}

	UDragWidget* DragDropOperation = NewObject<UDragWidget>();
	this->SetVisibility(ESlateVisibility::HitTestInvisible);

	DragDropOperation->WidgetReference = this;
	DragDropOperation->WidgetSlotReference = Cast<UCardSlotWidget>(this->GetParent()->GetOuter()->GetOuter());
	DragDropOperation->ChildIndexReference = DragDropOperation->WidgetSlotReference->GetParent()->GetChildIndex(DragDropOperation->WidgetSlotReference);
	DragDropOperation->DefaultDragVisual = this;
	DragDropOperation->Pivot = EDragPivot::MouseDown;

	OutOperation = DragDropOperation;
}

void UUnitIconWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UDragWidget* DragWidgetResult = Cast<UDragWidget>(InOperation);
	if (IsValid(DragWidgetResult) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UUnitIconWidget::NativeOnDragLeave IsValid(DragWidgetResult) == false"));
		return;
	}
	if (this == DragWidgetResult->WidgetReference)
	{
		RemoveFromParent();
	}
}

void UUnitIconWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UDragWidget* DragWidgetResult = Cast<UDragWidget>(InOperation);
	if (IsValid(DragWidgetResult) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UUnitIconWidget::NativeOnDragCancelled IsValid(DragWidgetResult) == false"));
		return;
	}
	UUnitIconWidget* Card = Cast<UUnitIconWidget>(DragWidgetResult->WidgetReference);
	if (IsValid(Card) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UUnitIconWidget::NativeOnDragCancelled IsValid(Card) == false"));
		return;
	}

	Card->SetVisibility(ESlateVisibility::Visible);

	if (IsValid(DragWidgetResult->WidgetSlotReference))
	{
		DragWidgetResult->WidgetSlotReference->SetCard(Card);
	}
}

void UUnitIconWidget::OnIconClicked()
{
	if (bHighlightOnSelected)
	{
		HighlightBorder->SetRenderOpacity(1 - HighlightBorder->GetRenderOpacity());
	}

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

void UUnitIconWidget::SetIsDraggable(bool NewValue)
{
	bCanBeDragged = NewValue;
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

void UUnitIconWidget::SetItemBrush(FSlateBrush ItemBrush)
{
	IMG_ItemIcon->SetBrush(ItemBrush);
}

FSlateBrush UUnitIconWidget::GetItemBrush()
{
	return IMG_ItemIcon->GetBrush();
}

void UUnitIconWidget::SetItemColor(FLinearColor Color)
{
	BTN_Icon->SetBackgroundColor(Color);
}

FLinearColor UUnitIconWidget::GetItemColor()
{
	return BTN_Icon->GetBackgroundColor();
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
