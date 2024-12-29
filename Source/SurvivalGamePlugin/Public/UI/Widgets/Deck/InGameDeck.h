// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameDeck.generated.h"

class UCardContainer;

UCLASS()
class SURVIVALGAMEPLUGIN_API UInGameDeck : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UCardContainer* EquipedCardsContainer;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UCardContainer* MainCardsContainer;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UCardContainer* SecondaryCardsContainer;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


public:

	UCardContainer* GetEquipedCardsContainer();

	UCardContainer* GetMainCardsContainer();

	UCardContainer* GetSecondaryCardsContainer();

};
