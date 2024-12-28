// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameDeck.generated.h"

class UWrapBox;
class UWidgetSwitcher;
class UUnitIconWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API UInGameDeck : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* EquipedCardsWrapBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* MainCardsWrapBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* SecondaryCardsWrapBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWidgetSwitcher* EquipedCardsWidgetSwitcher;

public:

	TArray<UWidget*> GetEquipedCards();

	TArray<UWidget*> GetMainHandCards();

	TArray<UWidget*> GetSecondaryHandCards();

	UFUNCTION()
	void AddUnitCardToEquipedCardsBox(UUnitIconWidget* ItemIconWidget);

	UFUNCTION()
	void AddUnitCardToMainCardsBox(UUnitIconWidget* ItemIconWidget);

	UFUNCTION()
	void AddUnitCardToSecondaryCardsBox(UUnitIconWidget* ItemIconWidget);

	UFUNCTION()
	void ClearEquipedCardsBox();

	UFUNCTION()
	void ClearMainCardsBox();

	UFUNCTION()
	void ClearSecondaryCardsBox();

	UFUNCTION()
	void SetEquipedCardsWidgetSwitcherActiveIndex(int Index);
};
