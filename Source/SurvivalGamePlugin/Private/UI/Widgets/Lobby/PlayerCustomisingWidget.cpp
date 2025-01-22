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

	for (UWidget* Child : DeckIconBox->GetAllChildren())
	{
		UDeckIcon* DeckIcon = Cast<UDeckIcon>(Child);
		if (IsValid(DeckIcon) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UPlayerCustomisingWidget::CycleDecks_Left IsValid(DeckIcon) == false"));
			continue;
		}

		int DeckNumber = DeckIconBox->GetChildIndex(Child) + 1;
		DeckIcon->SetDeckNumber(DeckNumber);
		DeckIcon->SetIsEnabled(DeckNumber <= 2);
		DeckIcon->IconClicked.AddDynamic(this, &UPlayerCustomisingWidget::SelectDeck);
	}

	BTN_CycleDecks_Left->OnClicked.AddDynamic(this, &UPlayerCustomisingWidget::CycleDecks_Left);
	BTN_CycleDecks_Right->OnClicked.AddDynamic(this, &UPlayerCustomisingWidget::CycleDecks_Right);

	TXTBTN_ReadyStatus->OnButtonClickedVoid.AddDynamic(this, &UPlayerCustomisingWidget::OnReadyButtonClicked);
	TXTBTN_LeaveTable->OnButtonClickedVoid.AddDynamic(this, &UPlayerCustomisingWidget::OnLeaveButtonClicked);
	TXTBTN_LeaveTable->SetText(FText::FromString("Leave Table"));
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

void UPlayerCustomisingWidget::SelectDeck(int DeckIndex)
{
	DeckSelected.Broadcast(DeckIndex);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, FString::Printf(TEXT("Selected Deck #%d"), DeckIndex));
}

void UPlayerCustomisingWidget::CycleDecks_Left()
{
	for (UWidget* Child : DeckIconBox->GetAllChildren())
	{
		UDeckIcon* DeckIcon = Cast<UDeckIcon>(Child);
		if (IsValid(DeckIcon) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UPlayerCustomisingWidget::CycleDecks_Left IsValid(DeckIcon) == false"));
			continue;
		}
		
		int NewDeckNumber = DeckIcon->GetDeckNumber() - 1;
		if (NewDeckNumber == 0)
		{
			NewDeckNumber = 5;
		}
		DeckIcon->SetDeckNumber(NewDeckNumber);
		DeckIcon->SetIsEnabled(NewDeckNumber <= 2);
	}
}

void UPlayerCustomisingWidget::CycleDecks_Right()
{
	for (UWidget* Child : DeckIconBox->GetAllChildren())
	{
		UDeckIcon* DeckIcon = Cast<UDeckIcon>(Child);
		if (IsValid(DeckIcon) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UPlayerCustomisingWidget::CycleDecks_Left IsValid(DeckIcon) == false"));
			continue;
		}

		int NewDeckNumber = DeckIcon->GetDeckNumber() + 1;
		if (NewDeckNumber == 6)
		{
			NewDeckNumber = 1;
		}
		DeckIcon->SetDeckNumber(NewDeckNumber);
		DeckIcon->SetIsEnabled(NewDeckNumber <= 2);
	}
}

void UPlayerCustomisingWidget::OnReadyButtonClicked()
{
	OnReadyButton.Broadcast();
}

void UPlayerCustomisingWidget::OnLeaveButtonClicked()
{
	OnLeaveButton.Broadcast();
	if (CardSelectorBorder->HasAnyChildren())
	{
		CardSelectorBorder->ClearChildren();
	}
}

void UPlayerCustomisingWidget::Config(EPlayerReadinessUIStatus PlayerReadiness)
{
	TXTBTN_ReadyStatus->SetText(FText::FromString(PlayerReadiness == EPlayerReadinessUIStatus::Ready ? "Cancel" : (PlayerReadiness == EPlayerReadinessUIStatus::NotReady ? "Ready" : "Updating Status")));
}

void UPlayerCustomisingWidget::OpenCardSelector(UCardSelectorWidget* CardSelector)
{
	if (CardSelectorBorder->HasAnyChildren())
	{
		CardSelectorBorder->ClearChildren();
	}
	CardSelectorBorder->AddChild(CardSelector);
}

void UPlayerCustomisingWidget::CloseWidget_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerCustomisingWidget::CloseWidget_Implementation No blueprint implementation exists for function"));
	RemoveFromParent();
}
