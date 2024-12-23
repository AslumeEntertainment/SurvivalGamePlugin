// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/UnitIconWidget.h"

#include "Headers/GeneralDelegates.h"

#include "ShopUnitWidgetBase.generated.h"

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class SURVIVALGAMEPLUGIN_API UShopUnitWidgetBase : public UUnitIconWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Cost;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	uint16 InvisibleCost = 0;

public: 

	UFUNCTION()
	void SetUnitCost(uint16 Cost = 1);

	UFUNCTION()
	void ConfigShopUnit(FText Name, UTexture2D* Image, FLinearColor Color, uint16 Cost = 1, float Scale = 1);

};
