// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "UnitIconWidget.generated.h"

class UImage;
class UBorder;
class UButton;
class USizeBox;
class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UUnitIconWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	bool bCanBeDragged = false;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Icon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* IMG_ItemIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_ItemName;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	USizeBox* UnitSizeBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UBorder* HighlightBorder;

	virtual FEventReply RedirectMouseDownToWidget(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION()
	void OnIconClicked();

	UFUNCTION()
	void BroadcastParentOnIconClicked();

public:

	bool bHighlightOnSelected = false;

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

	void ToggleHighlight();

	FIntegerTransferSignature IconClicked;
	FPanelWidgetTransferSignature BroadcastParentOnClicked;

};
