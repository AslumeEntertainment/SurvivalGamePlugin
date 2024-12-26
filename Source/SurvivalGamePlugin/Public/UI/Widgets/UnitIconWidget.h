// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "UnitIconWidget.generated.h"

class UImage;
class UButton;
class USizeBox;
class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UUnitIconWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void OnIconClicked();

	UFUNCTION()
	void BroadcastParentOnIconClicked();

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Icon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* IMG_ItemIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_ItemName;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	USizeBox* UnitSizeBox;

public:

	virtual void NativeOnInitialized() override;

	virtual void SetIsDraggable(bool NewValue);

	UFUNCTION(BlueprintCallable)
	void SetItemName(FText ItemName);

	UFUNCTION(BlueprintCallable)
	FString GetItemName();

	UFUNCTION(BlueprintCallable)
	void SetItemImage(UTexture2D* ItemImage);

	UFUNCTION(BlueprintCallable)
	void SetItemBrush(FSlateBrush ItemBrush);

	UFUNCTION(BlueprintCallable)
	FSlateBrush GetItemBrush();

	UFUNCTION(BlueprintCallable)
	void SetItemColor(FLinearColor Color);

	UFUNCTION(BlueprintCallable)
	FLinearColor GetItemColor();

	UFUNCTION(BlueprintCallable)
	void SetUnitScale(float Scale);

	UFUNCTION(BlueprintCallable)
	void ConfigUnit(FText Name, UTexture2D* Image, FLinearColor Color, float Scale);

	FIntegerTransferSignature IconClicked;
	FPanelWidgetTransferSignature BroadcastParentOnClicked;

};
