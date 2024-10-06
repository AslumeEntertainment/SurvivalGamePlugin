// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "ItemIconWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UItemIconWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void OnIconClicked();

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Icon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* IMG_ItemIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_ItemName;

public:

	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void SetItemName(FText ItemName);

	UFUNCTION()
	void SetItemImage(UTexture2D* ItemImage);

	FTextTransferSignature IconClicked;

};
