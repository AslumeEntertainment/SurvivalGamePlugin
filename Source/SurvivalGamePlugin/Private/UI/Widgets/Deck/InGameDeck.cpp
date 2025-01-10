// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/InGameDeck.h"

#include "Components/PanelWidget.h"

#include "UI/Widgets/Deck/CardContainer.h"
#include "UI/DragWidget.h"

bool UInGameDeck::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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

UCardContainer* UInGameDeck::GetEquipedCardsContainer()
{
	return EquipedCardsContainer;
}

UCardContainer* UInGameDeck::GetMainCardsContainer()
{
	return MainCardsContainer;
}

UCardContainer* UInGameDeck::GetSecondaryCardsContainer()
{
	return SecondaryCardsContainer;
}
