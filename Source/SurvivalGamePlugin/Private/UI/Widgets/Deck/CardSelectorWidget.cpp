// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/CardSelectorWidget.h"

#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

#include "UI/Widgets/UnitIconWidget.h"

void UCardSelectorWidget::MarkCardAsSelected(int Index)
{
	if (SelectedCards.Contains(Index))
	{
		SelectedCards.Remove(Index);
		return;
	}

	SelectedCards.Add(Index);

	if (SelectedCards.Num() == NumberOfCardsToSelect || SelectedCards.Num() == CardWrapBox->GetChildrenCount())
	{
		if (IsValid(CallerWidget) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UCardSelectorWidget::MarkCardAsSelected IsValid(CallerWidget) == false"));
			return;
		}
		if (CallerWidget->HasAnyChildren())
		{
			CallerWidget->ClearChildren();
		}
		OnNeededCardsSelected.Broadcast(SelectedCards, CallerWidget);
		
		RemoveFromParent();
	}
}

void UCardSelectorWidget::ConfigCardSelector(UPanelWidget* InCallerWidget, int CardsNeeded)
{
	CallerWidget = InCallerWidget;
	NumberOfCardsToSelect = CardsNeeded;
}

void UCardSelectorWidget::SetTitle(FString Title)
{
	TXT_Title->SetText(FText::FromString(Title));
}

void UCardSelectorWidget::FillCards(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors)
{
	UUnitIconWidget* TempCard = nullptr;

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
		CardWrapBox->AddChildToWrapBox(TempCard);
	}
}

void UCardSelectorWidget::AddCardToSelector(UWidget* CardWidget)
{
	CardWrapBox->AddChildToWrapBox(CardWidget);
}

void UCardSelectorWidget::EmptyCardSelector()
{
	if (CardWrapBox->HasAnyChildren())
	{
		CardWrapBox->ClearChildren();
	}
}
