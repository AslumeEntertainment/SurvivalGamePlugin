// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardSlotWidget.generated.h"

class UBorder;
class UUnitIconWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API UCardSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UBorder* SlotBorder;

public:

	bool IsEmpty() const;

	void Empty();

	void SetCard(UUnitIconWidget* Card);

	UUnitIconWidget* GetCard();

};
