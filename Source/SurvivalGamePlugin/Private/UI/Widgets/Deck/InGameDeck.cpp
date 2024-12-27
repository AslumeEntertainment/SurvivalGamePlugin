// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/InGameDeck.h"

#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"

#include "UI/Widgets/UnitIconWidget.h"

void UInGameDeck::AddUnitCardToEquipedCardsBox(UUnitIconWidget* ItemIconWidget)
{
	EquipedCardsWrapBox->AddChildToWrapBox(ItemIconWidget);
}

void UInGameDeck::AddUnitCardToMainCardsBox(UUnitIconWidget* ItemIconWidget)
{
	MainCardsWrapBox->AddChildToWrapBox(ItemIconWidget);
}

void UInGameDeck::AddUnitCardToSecondaryCardsBox(UUnitIconWidget* ItemIconWidget)
{
	SecondaryCardsWrapBox->AddChildToWrapBox(ItemIconWidget);
}

void UInGameDeck::ClearEquipedCardsBox()
{
	if (EquipedCardsWrapBox->HasAnyChildren())
	{
		EquipedCardsWrapBox->ClearChildren();
	}
}

void UInGameDeck::ClearMainCardsBox()
{
	if (MainCardsWrapBox->HasAnyChildren())
	{
		MainCardsWrapBox->ClearChildren();
	}
}

void UInGameDeck::ClearSecondaryCardsBox()
{
	if (SecondaryCardsWrapBox->HasAnyChildren())
	{
		SecondaryCardsWrapBox->ClearChildren();
	}
}

void UInGameDeck::SetEquipedCardsWidgetSwitcherActiveIndex(int Index)
{
	EquipedCardsWidgetSwitcher->SetActiveWidgetIndex(Index);
}
