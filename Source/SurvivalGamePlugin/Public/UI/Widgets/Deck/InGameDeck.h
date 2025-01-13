// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
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
	UCardContainer* LeftVerticalCardsContainer;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UCardContainer* RightVerticalCardsContainer;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


public:
	
	UCardContainer* GetEquipedCardsContainer();

	UCardContainer* GetMainCardsContainer(bool bIsPlayerRightHanded);

	UCardContainer* GetSecondaryCardsContainer(bool bIsPlayerRightHanded);

	FOnCardDroppedSignature OnCardDropped;

};
