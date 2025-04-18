// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "WarningWidgetBase.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class SURVIVALGAMEPLUGIN_API UWarningWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Message;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Confirm;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_ConfirmText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Cancel;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_CancelText;

	

	UFUNCTION()
	void CancelButtonClicked();

public:


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ConfirmButtonClicked();

	void NativeOnInitialized() override;

	UFUNCTION()
	void ConfigWarning(FText Message, bool CanClickConfirm = true, FText ConfirmText = FText::FromString("Yes"), FText CancelText = FText::FromString("No"));

	FVoidDataTransferSignature OnConfirmButtonClicked;
	FVoidDataTransferSignature OnCancelButtonClicked;
	
};
