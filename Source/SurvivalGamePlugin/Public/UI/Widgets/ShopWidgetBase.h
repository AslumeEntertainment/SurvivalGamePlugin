// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "ShopWidgetBase.generated.h"

class UButton;
class UScrollBox;
class UTextBlock;
class UImage;
class UBoosterPackWidgetBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API UShopWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Exit;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Money;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UScrollBox* SCRB_BoosterPack;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UScrollBox* SCRB_Sell;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UBoosterPackWidgetBase> BoosterPackWidgetClass;

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnBoosterPackClicked(UUserWidget* Widget);

public:
	void NativeOnInitialized() override;

	void FillBoosters(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<uint16> Costs);
	void SetMoney(uint32 Money);
	FVoidDataTransferSignature OnExitButton;
	FIntegerTransferSignature OnPackSelected;
};
