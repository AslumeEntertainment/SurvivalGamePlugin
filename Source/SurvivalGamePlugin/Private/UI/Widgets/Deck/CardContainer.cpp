// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/CardContainer.h"

#include "Components/WrapBox.h"

#include "UI/Widgets/Core/CardSlotWidget.h"
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
		return OnCardDropped.Execute(DragWidgetResult->WidgetSlotReference, DragWidgetResult->ChildIndexReference);
	}

	return false;
}

void UCardContainer::InitContainer(int NumberOfCardsToHold)
{
	CardWrapBox->ClearChildren();

	UCardSlotWidget* CardSlot = nullptr;

	for (int i = 0; i < NumberOfCardsToHold; i++)
	{
		CardSlot = CreateWidget<UCardSlotWidget>(this, CardSlotWidgetClass);
		if (IsValid(CardSlot))
		{
			CardWrapBox->AddChild(CardSlot);
		}
	}
}

TArray<UUnitIconWidget*> UCardContainer::GetCards()
{
	TArray<UUnitIconWidget*> Cards;
	for (UWidget* Child : CardWrapBox->GetAllChildren())
	{
		UCardSlotWidget* CardSlot = Cast<UCardSlotWidget>(Child);
		if (IsValid(CardSlot) && IsValid(CardSlot->GetCard()))
		{
			Cards.Add(CardSlot->GetCard());
		}
	}

	return Cards;
}

bool UCardContainer::ContainsCard(UUnitIconWidget* Card)
{
	if (IsValid(Card) == false)
	{
		return false;
	}
	for (UWidget* Child : CardWrapBox->GetAllChildren())
	{
		UCardSlotWidget* CardSlot = Cast<UCardSlotWidget>(Child);
		if (IsValid(CardSlot) == false)
		{
			continue;
		}
		if (CardSlot->GetCard() == Card)
		{
			return true;
		}
	}
	return false;
}

bool UCardContainer::ContainsSlot(UCardSlotWidget* CardSlot)
{
	if (IsValid(CardSlot) == false)
	{
		return false;
	}
	for (UWidget* Child : CardWrapBox->GetAllChildren())
	{
		UCardSlotWidget* ChildCardSlot = Cast<UCardSlotWidget>(Child);
		if (IsValid(ChildCardSlot) && ChildCardSlot == CardSlot)
		{
			return true;
		}
	}
	return false;
}

UUnitIconWidget* UCardContainer::AddCard(FText Name, UTexture2D* Image, FLinearColor Color)
{
	for (UWidget* Child : CardWrapBox->GetAllChildren())
	{
		UCardSlotWidget* CardSlot = Cast<UCardSlotWidget>(Child);
		if (IsValid(CardSlot) && CardSlot->IsEmpty())
		{
			UUnitIconWidget* Card = CreateWidget<UUnitIconWidget>(this, CardWidgetClass);
			if (IsValid(Card) == false)
			{
				return nullptr;
			}
			Card->ConfigUnit(Name, Image, Color, 1);
			CardSlot->SetCard(Card);

			return Card;
		}
	}

	return nullptr;
}

void UCardContainer::ClearCards()
{
	for (UWidget* Child : CardWrapBox->GetAllChildren())
	{
		UCardSlotWidget* CardSlot = Cast<UCardSlotWidget>(Child);
		if (IsValid(CardSlot))
		{
			CardSlot->Empty();
		}
	}
}

void UCardContainer::ClearSlots()
{
	CardWrapBox->ClearChildren();
}
