// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Headers/DeckSegmentsEnum.h"

#include "CardSelectorWidget.generated.h"

class UUnitIconWidget;
class UPanelWidget;
class UTextBlock;
class UWrapBox;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNeededCardsSelectedSignature, TArray<uint16>, SelectedIndexes, EDeckSegment, Segment);

UCLASS()
class SURVIVALGAMEPLUGIN_API UCardSelectorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TArray<uint16> CardIndexes;

	UPROPERTY()
	EDeckSegment CallingSegment;

	UPROPERTY()
	uint8 NumberOfCardsToSelect = 0;

	UPROPERTY()
	TArray<uint16> SelectedCards;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Title;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* CardWrapBox;

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UUnitIconWidget> CardWidgetClass;

	UFUNCTION()
	void MarkCardAsSelected(int Index);

	void ToggleCardHighlight(uint8 Index);

public:

	void ConfigCardSelector(EDeckSegment Segment, uint8 CardsNeeded, TArray<uint16> AlredySelectedCards);

	void SetTitle(FText Title);

	void FillCards(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors, TArray<uint16> InCardIndexes);

	void EmptyCardSelector();

	FOnNeededCardsSelectedSignature OnNeededCardsSelected;
};
