// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "CardContainer.generated.h"

class UWrapBox;
class UPanelWidget;
class UUnitIconWidget;
class UCardSlotWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API UCardContainer : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* CardWrapBox;

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UUnitIconWidget> CardWidgetClass;

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UCardSlotWidget> CardSlotWidgetClass;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:

	void InitContainer(int NumberOfCardsToHold);

	TArray<UUnitIconWidget*> GetCards();

	bool ContainsCard(UUnitIconWidget* Card);

	bool ContainsSlot(UCardSlotWidget* CardSlot);

	UFUNCTION()
	UUnitIconWidget* AddCard(FText Name, UTexture2D* Image, FLinearColor Color);

	UFUNCTION()
	void ClearCards();

	void ClearSlots();

	FOnCardDroppedSignature OnCardDropped;

};
