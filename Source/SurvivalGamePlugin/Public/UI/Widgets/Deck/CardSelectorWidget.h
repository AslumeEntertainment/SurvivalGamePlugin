// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardSelectorWidget.generated.h"

class UUnitIconWidget;
class UTextBlock;
class UWrapBox;

UCLASS()
class SURVIVALGAMEPLUGIN_API UCardSelectorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPanelWidget* CallerWidget = nullptr;

	int NumberOfCardsToSelect = 0;

	TArray<int> SelectedCards;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Title;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* CardWrapBox;

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UUnitIconWidget> CardWidgetClass;

	UFUNCTION()
	void MarkCardAsSelected(int Index);

public:

	void ConfigCardSelector(UPanelWidget* InCallerWidget, int CardsNeeded);

	void SetTitle(FString Title);

	void FillCards(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors);

	void AddCardToSelector(UWidget* CardWidget);

	void EmptyCardSelector();
};
