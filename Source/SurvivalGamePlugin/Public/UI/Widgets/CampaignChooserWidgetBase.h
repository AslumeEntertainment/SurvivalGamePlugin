// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "CampaignChooserWidgetBase.generated.h"

class UButton;
class UWrapBox;
class UShopUnitWidgetBase;


UCLASS()
class SURVIVALGAMEPLUGIN_API UCampaignChooserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_Exit;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* WRPB_Campaigns;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UShopUnitWidgetBase> ShopUnitWidgetClass;

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnCampaignClicked(UUserWidget* Widget);

public:
	void NativeOnInitialized() override;

	UFUNCTION()
	void FillCampaigns(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor>Colors, TArray<uint8> PlayerNums);

	UFUNCTION()
	void Show();

	UFUNCTION()
	void Hide();

	UFUNCTION()
	void MarkForDestruction();

	FVoidDataTransferSignature OnExitButton;
	FIntegerTransferSignature OnCampaignSelected;
};
