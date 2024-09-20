// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "LobbyManagingWidgetBase.generated.h"

class UButton;
class UWrapBox;
class UScrollBox;
class UTextBlock;
class UImage;
class UWidgetSwitcher;

UCLASS()
class SURVIVALGAMEPLUGIN_API ULobbyManagingWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	uint8 CurrentActiveWidget = 0;

	UPROPERTY()
	bool SupervisedButtonState = 0;

	UPROPERTY()
	bool UnsupervisedButtonState = 0;

	UPROPERTY(EditAnywhere, Category = "Menu", meta = (BindWidget))
	UButton* BTN_Exit;

	UPROPERTY(EditAnywhere, Category = "Menu", meta = (BindWidget))
	UButton* BTN_Leave;

	UPROPERTY(EditAnywhere, Category = "Menu", meta = (BindWidget))
	UButton* BTN_ChooseCampaign;

	UPROPERTY(EditAnywhere, Category = "Menu", meta = (BindWidget))
	UButton* BTN_ChangeCampaign;

	UPROPERTY(EditAnywhere, Category = "Menu", meta = (BindWidget))
	UButton* BTN_StartCampaign;

	UPROPERTY(EditAnywhere, Category = "Menu", meta = (BindWidget))
	UButton* BTN_StopCampaign;

	UPROPERTY(EditAnywhere, Category = "Menu", meta = (BindWidget))
	UWidgetSwitcher* WSW_CampaignWindow;

	UPROPERTY(EditAnywhere, Category = "Campaign", meta = (BindWidget))
	UButton* BTN_Supervised;

	UPROPERTY(EditAnywhere, Category = "Campaign", meta = (BindWidget))
	UButton* BTN_Unsupervised;

	UPROPERTY(EditAnywhere, Category = "Campaign", meta = (BindWidget))
	UTextBlock* TXT_Name;

	UPROPERTY(EditAnywhere, Category = "Campaign", meta = (BindWidget))
	UTextBlock* TXT_PlayerNum;

	UPROPERTY(EditAnywhere, Category = "Campaign", meta = (BindWidget))
	UTextBlock* TXT_Supervised;

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnLeaveButtonClicked();

	UFUNCTION()
	void OnChooseCampaignClicked();

	UFUNCTION()
	void OnStartCampaignClicked();

	UFUNCTION()
	void OnStopCampaignClicked();

	UFUNCTION()
	void OnSupervisedClicked();

	UFUNCTION()
	void OnUnsupervisedClicked();

public:
	void NativeOnInitialized() override;

	void NativeConstruct() override;

	UFUNCTION()
	void FillCampaign(FText Name, uint8 PlayerNum, bool IsSupervised, bool IsChangeable);

	void ResetCampaign();

	UFUNCTION()
	void Show();

	UFUNCTION()
	void Hide();

	UFUNCTION()
	void MarkForDestruction();

	UFUNCTION()
	void SetSupervised(bool State);

	FVoidDataTransferSignature OnExitButton;
	FVoidDataTransferSignature OnLeaveButton;
	FVoidDataTransferSignature OnChooseCampaignButton;
	FVoidDataTransferSignature OnStartCampaignButton;
	FVoidDataTransferSignature OnStopCampaignButton;
	FBooleanTransferSignature OnSupervisedChanged;
};
