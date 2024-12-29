// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/CardContainer.h"

#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"

#include "UI/Widgets/UnitIconWidget.h"

bool UCardContainer::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("A widget was dropped in %s"), *this->GetName()));
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
