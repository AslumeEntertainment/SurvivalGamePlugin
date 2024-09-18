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
class UShopUnitWidgetBase;

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
	UScrollBox* SCRB_Heroes;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UScrollBox* SCRB_Resources;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UShopUnitWidgetBase> ShopUnitWidgetClass;

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnBoosterPackClicked(UUserWidget* Widget);

	UFUNCTION()
	void OnHeroCardClicked(UUserWidget* Widget);

	UFUNCTION()
	void OnResourceCardClicked(UUserWidget* Widget);

public:
	void NativeOnInitialized() override;

	void FillBoosters(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<uint16> Costs);
	void FillSells(TArray<FText> HeroNames, TArray<UTexture2D*> HeroImages, TArray<uint16> HeroCosts, TArray<FText> ResourceNames, TArray<UTexture2D*> ResourceImages, TArray<uint16> ResourceCosts);
	void SetMoney(uint32 Money);
	void RemoveResourceCard(uint16 Index);
	void RemoveHeroCard(uint16 Index);

	FVoidDataTransferSignature OnExitButton;
	FIntegerTransferSignature OnPackSelected;
	FIntegerTransferSignature OnHeroCardSelected;
	FIntegerTransferSignature OnResourceCardSelected;
};
