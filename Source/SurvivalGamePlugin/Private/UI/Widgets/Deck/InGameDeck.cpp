// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/InGameDeck.h"

#include "UI/Widgets/Deck/CardContainer.h"


bool UInGameDeck::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("A widget was dropped in %s"), *this->GetName()));
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
