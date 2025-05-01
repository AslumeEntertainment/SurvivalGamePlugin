// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/CardSelectorWidget.h"

#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

#include "UI/Widgets/Core/UnitIconWidget.h"

void UCardSelectorWidget::MarkCardAsSelected(int Index)
{
	uint16 CardPointingIndex = CardIndexes[Index];
	if (SelectedCards.Contains(CardPointingIndex))
	{
		SelectedCards.Remove(CardPointingIndex);
		
	}
	else
	{
		if (SelectedCards.Num() < NumberOfCardsToSelect)
		{
			SelectedCards.Add(CardPointingIndex);
			
		}
		/*else
		{
			SelectedCards[NumberOfCardsToSelect - 1] = CardPointingIndex;// ako iskame avtomatichen selestio nan nqkva karta
		}*/
	}
	//ToggleCardHighlight(Index);
	OnNeededCardsSelected.Broadcast(SelectedCards, CallingSegment);
}

void UCardSelectorWidget::ToggleCardHighlight(uint8 Index)
{

}

void UCardSelectorWidget::ConfigCardSelector(EDeckSegment Segment, uint8 CardsNeeded, TArray<uint16> AlredySelectedCards)
{
	SelectedCards = AlredySelectedCards;
	SelectedCards.Remove(-1);
	CallingSegment = Segment;
	NumberOfCardsToSelect = CardsNeeded;
}

void UCardSelectorWidget::SetTitle(FText Title)
{
	TXT_Title->SetText(Title);
}

void UCardSelectorWidget::FillCards(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors, TArray<uint16> InCardIndexes)
{
	UUnitIconWidget* TempCard = nullptr;

	CardIndexes = InCardIndexes;

	if (Names.Num() != Images.Num() || Names.Num() != Colors.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UCardSelectorWidget::FillCards Names, Images and Colors have different sizes!"));
		return;
	}

	EmptyCardSelector();

	for (int i = 0; i < Names.Num(); i++)
	{
		TempCard = CreateWidget<UUnitIconWidget>(this, CardWidgetClass);
		if (IsValid(TempCard) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UCardSelectorWidget::FillCards IsValid(TempCard) == false"));
			return;
		}
		TempCard->ConfigUnit(Names[i], Images[i], Colors[i], 1);
		TempCard->IconClicked.AddDynamic(this, &UCardSelectorWidget::MarkCardAsSelected);
		TempCard->bHighlightOnSelected = true;

		if (SelectedCards.Contains(CardIndexes[i])) TempCard->ToggleHighlight();

		CardWrapBox->AddChildToWrapBox(TempCard);
	}
}

void UCardSelectorWidget::EmptyCardSelector()
{
	if (CardWrapBox->HasAnyChildren())
	{
		CardWrapBox->ClearChildren();
	}
}
