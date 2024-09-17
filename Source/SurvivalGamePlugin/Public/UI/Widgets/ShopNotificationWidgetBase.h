// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "ShopNotificationWidgetBase.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class SURVIVALGAMEPLUGIN_API UShopNotificationWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Action;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Object;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Cost;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Ok;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Cancel;

	UFUNCTION()
	void OnOKButtonClicked();

	UFUNCTION()
	void OnCancelButtonClicked();

public:
	void NativeOnInitialized() override;

	UFUNCTION()
	void Config(bool CanClickOk, FText Action, FText Object, uint16 Cost);

	FVoidDataTransferSignature OnOkButton;
	FVoidDataTransferSignature OnCancelButton;
	
};
