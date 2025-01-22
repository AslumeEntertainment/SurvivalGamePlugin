// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/DeckSegmentPanel.h"

#include "UI/Widgets/Core/UnitIconWidget.h"

void UDeckSegmentPanel::ReceiveCardClicked(int Index)
{
	OnCardClicked.Broadcast(Segment);
}

void UDeckSegmentPanel::Init(EDeckSegment InSegment)
{
	Segment = InSegment;
}

void UDeckSegmentPanel::FillSegment(TArray<FText> CardNames, TArray<UTexture2D*> CardIcons, TArray<FLinearColor> CardColors)
{
	if (HasAnyChildren())
	{
		ClearChildren();
	}

	for (int i=0; i<CardNames.Num(); i++)
	{
		UUnitIconWidget* ResourceCard = CreateWidget<UUnitIconWidget>(this, ResourceCardWidgetClass);
		if (IsValid(ResourceCard) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UDeckSegmentPanel::FillSegment IsValid(ResourceCard) == false"));
			return;
		}
		ResourceCard->ConfigUnit(CardNames[i], CardIcons[i], CardColors[i], 1);
		ResourceCard->IconClicked.AddDynamic(this, &UDeckSegmentPanel::ReceiveCardClicked);
		AddChild(ResourceCard);
	}

}
