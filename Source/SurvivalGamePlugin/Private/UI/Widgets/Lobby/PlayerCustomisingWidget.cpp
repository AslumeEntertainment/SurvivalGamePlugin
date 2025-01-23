// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/PlayerCustomisingWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/WrapBox.h"
#include "Components/Border.h"
#include "Components/Button.h"

#include "UI/Widgets/Core/TextButtonWidgetBase.h"
#include "UI/Widgets/Deck/CardSelectorWidget.h"
#include "UI/Widgets/Deck/HeroCardSelector.h"
#include "UI/Widgets/Deck/DeckIcon.h"
#include "UI/Widgets/Deck/DeckSegmentPanel.h"
#include "UI/Widgets/Core/UnitIconWidget.h"


void UPlayerCustomisingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BTN_CycleDecks_Left->OnClicked.AddDynamic(this, &UPlayerCustomisingWidget::CycleDecks_Left);
	BTN_CycleDecks_Right->OnClicked.AddDynamic(this, &UPlayerCustomisingWidget::CycleDecks_Right);

	TXTBTN_ReadyStatus->OnButtonClickedVoid.AddDynamic(this, &UPlayerCustomisingWidget::OnReadyButtonClicked);
	TXTBTN_LeaveTable->OnButtonClickedVoid.AddDynamic(this, &UPlayerCustomisingWidget::OnLeaveButtonClicked);
	TXTBTN_LeaveTable->SetText(FText::FromString("Leave Table"));
}

void UPlayerCustomisingWidget::InitDecks(uint8 InActiveDeckIndex, TArray<UTexture2D*> InDeckIcons)
{
	ActiveDeckIndex = InActiveDeckIndex;
	uint8 CenterIndex = NumDecksOnDisplay / 2;

	for (int i = 0; i < InDeckIcons.Num(); i++)
	{
		UDeckIcon* DeckIcon = CreateWidget<UDeckIcon>(this, DeckIconClass);
		if (IsValid(DeckIcon) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UDeckSegmentPanel::InitDecks IsValid(DeckIcon) == false"));
			return;
		}
		DeckIcon->SetDeckNumber(i + 1);
		DeckIcons.Add(DeckIcon);
	}

	RearangeIconBox();

}

void UPlayerCustomisingWidget::RearangeIconBox()
{
	DeckIconBox->ClearChildren();
	for (int i = ActiveDeckIndex - NumDecksOnDisplay / 2; i <= ActiveDeckIndex + NumDecksOnDisplay / 2; i++)
	{
		int IndexToAdd = i;
		while(IndexToAdd < 0)
		{
			IndexToAdd = DeckIcons.Num() + i;
		}
		while(IndexToAdd >= DeckIcons.Num())
		{
			IndexToAdd =  i - DeckIcons.Num();
		}
		DeckIconBox->AddChild(DeckIcons[IndexToAdd]);
	}
	SelectDeck();
}

void UPlayerCustomisingWidget::CycleDecks_Left()
{
	ActiveDeckIndex -= 1;
	if (ActiveDeckIndex < 0) ActiveDeckIndex = DeckIcons.Num() - 1;
	RearangeIconBox();
}

void UPlayerCustomisingWidget::CycleDecks_Right()
{
	ActiveDeckIndex += 1;
	if (ActiveDeckIndex == DeckIcons.Num()) ActiveDeckIndex = 0;
	RearangeIconBox();
}

void UPlayerCustomisingWidget::SelectDeck()
{
	DeckSelected.Broadcast(ActiveDeckIndex);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, FString::Printf(TEXT("Selected Deck #%d"), ActiveDeckIndex));
}

UDeckSegmentPanel* UPlayerCustomisingWidget::GetHeroCardPanel()
{
	return HeroCardBox;
}

UDeckSegmentPanel* UPlayerCustomisingWidget::GetMainCardsPanel()
{
	return MainHandCardsBox;
}

UDeckSegmentPanel* UPlayerCustomisingWidget::GetSecondaryCardsPanel()
{
	return SecondaryHandCardsBox;
}

void UPlayerCustomisingWidget::OnReadyButtonClicked()
{
	CloseCardSelector();
	OnReadyButton.Broadcast();
}

void UPlayerCustomisingWidget::OnLeaveButtonClicked()
{
	DeckIcons.Empty();
	OnLeaveButton.Broadcast();
	CloseCardSelector();
}

void UPlayerCustomisingWidget::Config(EPlayerReadinessUIStatus PlayerReadiness)
{
	if (PlayerReadiness == EPlayerReadinessUIStatus::NotReady)
	{
		HeroCardBox->SetIsEnabled(true);
		MainHandCardsBox->SetIsEnabled(true);
		SecondaryHandCardsBox->SetIsEnabled(true);

		TXTBTN_ReadyStatus->SetText(FText::FromString("Ready"));
	}
	else
	{
		HeroCardBox->SetIsEnabled(false);
		MainHandCardsBox->SetIsEnabled(false);
		SecondaryHandCardsBox->SetIsEnabled(false);
		if (PlayerReadiness == EPlayerReadinessUIStatus::Ready)
		{
			TXTBTN_ReadyStatus->SetText(FText::FromString("Canle"));
		}
		else
		{
			TXTBTN_ReadyStatus->SetText(FText::FromString("Updating Status"));
		}
	}

}

void UPlayerCustomisingWidget::OpenCardSelector(UCardSelectorWidget* CardSelector)
{
	CloseCardSelector();
	CardSelectorBorder->AddChild(CardSelector);
}

void UPlayerCustomisingWidget::CloseCardSelector()
{
	if (CardSelectorBorder->HasAnyChildren())
	{
		CardSelectorBorder->ClearChildren();
	}
}

void UPlayerCustomisingWidget::DeckPlayabilityChanged(bool DeckPlayability)
{
	TXTBTN_ReadyStatus->SetIsEnabled(DeckPlayability);
}

void UPlayerCustomisingWidget::CloseWidget_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerCustomisingWidget::CloseWidget_Implementation No blueprint implementation exists for function"));
	RemoveFromParent();
}
