// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerCustomisingWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/WrapBox.h"
#include "Components/Border.h"
#include "Components/Button.h"

#include "UI/Widgets/Core/TextButtonWidgetBase.h"
#include "UI/Widgets/Deck/CardSelectorWidget.h"
#include "UI/Widgets/UnitIconWidget.h"
#include "UI/Widgets/Deck/DeckIcon.h"


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

UPanelWidget* UPlayerCustomisingWidget::GetHeroCardPanel()
{
	return HeroCardBorder;
}

UPanelWidget* UPlayerCustomisingWidget::GetMainCardsPanel()
{
	return MainHandCardsBox;
}

UPanelWidget* UPlayerCustomisingWidget::GetSecondaryCardsPanel()
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
}

void UPlayerCustomisingWidget::Config(EPlayerReadinessUIStatus PlayerReadiness)
{
	TXTBTN_ReadyStatus->SetText(FText::FromString(PlayerReadiness == EPlayerReadinessUIStatus::Ready ? "Cancel" : (PlayerReadiness == EPlayerReadinessUIStatus::NotReady ? "Ready" : "Updating Status")));
}

UUnitIconWidget* UPlayerCustomisingWidget::FillHeroCardSlot(FText Name, UTexture2D* Image, FLinearColor Color)
{
	UUnitIconWidget* HeroCard = CreateWidget<UUnitIconWidget>(this, HeroCardWidgetClass);
	if (IsValid(HeroCard) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerCustomisingWidget::FillHeroCardSlot IsValid(HeroCard) == false"));
		return nullptr;
	}
	HeroCard->ConfigUnit(Name, Image, Color, 1);
	
	if (HeroCardBorder->HasAnyChildren())
	{
		HeroCardBorder->ClearChildren();
	}
	HeroCardBorder->AddChild(HeroCard);

	return HeroCard;
}

TArray<UUnitIconWidget*> UPlayerCustomisingWidget::FillMainHandCardsSlots(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors)
{
	if (Names.Num() != Images.Num() || Names.Num() != Colors.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerCustomisingWidget::FillMainHandCardsSlots Names, Images and Colors have different sizes!"));
		return TArray<UUnitIconWidget*>();
	}

	if (MainHandCardsBox->HasAnyChildren())
	{
		MainHandCardsBox->ClearChildren();
	}

	UUnitIconWidget* TempCard = nullptr;
	TArray<UUnitIconWidget*> AddedCards;

	for (int i = 0; i < Names.Num(); i++)
	{
		TempCard = CreateWidget<UUnitIconWidget>(this, ResourceCardWidgetClass);
		if (IsValid(TempCard) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UPlayerCustomisingWidget::FillMainHandCardsSlots IsValid(TempCard) == false"));
			return TArray<UUnitIconWidget*>();
		}
		TempCard->ConfigUnit(Names[i], Images[i], Colors[i], 1);
		MainHandCardsBox->AddChildToWrapBox(TempCard);
		AddedCards.Add(TempCard);
	}

	return AddedCards;
}

TArray<UUnitIconWidget*> UPlayerCustomisingWidget::FillSecondaryHandCardsSlots(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors)
{
	if (Names.Num() != Images.Num() || Names.Num() != Colors.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerCustomisingWidget::FillSecondaryHandSlots Names, Images and Colors have different sizes!"));
		return TArray<UUnitIconWidget*>();
	}

	if (SecondaryHandCardsBox->HasAnyChildren())
	{
		SecondaryHandCardsBox->ClearChildren();
	}

	UUnitIconWidget* TempCard = nullptr;
	TArray<UUnitIconWidget*> AddedCards;

	for (int i = 0; i < Names.Num(); i++)
	{
		TempCard = CreateWidget<UUnitIconWidget>(this, ResourceCardWidgetClass);
		if (IsValid(TempCard) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UPlayerCustomisingWidget::FillSecondaryHandSlots IsValid(TempCard) == false"));
			return TArray<UUnitIconWidget*>();
		}
		TempCard->ConfigUnit(Names[i], Images[i], Colors[i], 1);
		SecondaryHandCardsBox->AddChildToWrapBox(TempCard);
		AddedCards.Add(TempCard);
	}

	return AddedCards;
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
