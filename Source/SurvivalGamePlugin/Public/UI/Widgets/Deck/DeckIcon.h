// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Headers/GeneralDelegates.h"

#include "DeckIcon.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UDeckIcon : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UFUNCTION()
	void OnIconClicked();

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Icon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_DeckNumber;

public:

	virtual void NativeOnInitialized() override;

	void SetIsEnabled(bool NewValue);

	void SetDeckNumber(int DeckNumber);

	int GetDeckNumber();

	FIntegerTransferSignature IconClicked;
};
