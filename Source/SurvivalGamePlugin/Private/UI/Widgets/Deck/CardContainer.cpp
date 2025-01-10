// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/CardContainer.h"

#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"

#include "UI/Widgets/UnitIconWidget.h"
#include "UI/DragWidget.h"

bool UCardContainer::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UDragWidget* DragWidgetResult = Cast<UDragWidget>(InOperation);
	if (IsValid(DragWidgetResult) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UCardContainer::NativeOnDrop IsValid(DragWidgetResult) == false"));
		return false;
	}

	if (OnCardDropped.IsBound())
	{
		return OnCardDropped.Execute(DragWidgetResult->ParentReference, DragWidgetResult->ChildIndexReference);
	}

	return false;
}

TArray<UUnitIconWidget*> UCardContainer::GetCards()
{
	TArray<UUnitIconWidget*> Cards;
	for (UWidget* Child : CardWrapBox->GetAllChildren())
	{
		UUnitIconWidget* Card = Cast<UUnitIconWidget>(Child);
		if (IsValid(Card))
		{
			Cards.Add(Card);
		}
	}

	return Cards;
}

bool UCardContainer::IsThisMyCardPanel(UWidget* CardPanel)
{
	return (CardPanel == CardWrapBox);
}

UUnitIconWidget* UCardContainer::AddCard(FText Name, UTexture2D* Image, FLinearColor Color)
{
	UUnitIconWidget* Card = CreateWidget<UUnitIconWidget>(this, CardWidgetClass);
	if (IsValid(Card))
	{
		Card->ConfigUnit(Name, Image, Color, 1);
		CardWrapBox->AddChild(Card);
		CardWidgetSwitcher->SetActiveWidgetIndex(1);
	}
	return Card;
}

void UCardContainer::ClearCards()
{
	if (CardWrapBox->HasAnyChildren())
	{
		CardWrapBox->ClearChildren();
		CardWidgetSwitcher->SetActiveWidgetIndex(0);
	}
}
