// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WrapBox.h"

#include "Headers/DeckSegmentsEnum.h"

#include "DeckSegmentPanel.generated.h"

class UUnitIconWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardClickedSignature, EDeckSegment, SegmentToUpdate);

UCLASS()
class SURVIVALGAMEPLUGIN_API UDeckSegmentPanel : public UWrapBox
{
	GENERATED_BODY()
	
private:
	EDeckSegment Segment;

	UFUNCTION()
	void ReceiveCardClicked(int Index);

protected:

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UUnitIconWidget> ResourceCardWidgetClass;

public:

	void Init(EDeckSegment InSegment);

	void FillSegment(TArray<FText> CardNames, TArray<UTexture2D*> CardIcons, TArray<FLinearColor> CardColors);

	FOnCardClickedSignature OnCardClicked;
};
