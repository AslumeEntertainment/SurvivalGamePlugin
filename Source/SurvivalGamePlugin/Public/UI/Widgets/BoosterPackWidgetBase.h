// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Headers/GeneralDelegates.h"

#include "BoosterPackWidgetBase.generated.h"

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class SURVIVALGAMEPLUGIN_API UBoosterPackWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Icon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* IMG_Icon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Name;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Cost;

	UFUNCTION()
	void OnBodyButtonClicked();

public:
	void NativeOnInitialized() override;

	UFUNCTION()
	void Config(FText Name, UTexture2D* Image, uint16 Cost);

	FWidgetTransferSignature OnBodyButton;

};
