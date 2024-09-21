// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "TextButtonWidgetBase.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UTextButtonWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT;

	UFUNCTION()
	void OnButtonClicked();

public:

	void NativeOnInitialized() override;

	void SetText(FText Text);

	UFUNCTION()
	void SetEnabledStatus(bool State);

	FVoidDataTransferSignature OnButtonClickedVoid;
	FWidgetTransferSignature OnButtonClickedSelf;
};
