// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "PlayerCustomisingWidget.generated.h"

class UTextButtonWidgetBase;

UENUM(BlueprintType)
enum class EPlayerReadinessUIStatus : uint8
{
	Ready,
	UpdatingStatus,
	NotReady,
};


UCLASS()
class SURVIVALGAMEPLUGIN_API UPlayerCustomisingWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextButtonWidgetBase* TXTBTN_ReadyStatus;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextButtonWidgetBase* TXTBTN_LeaveTable;

	UFUNCTION()
	void OnReadyButtonClicked();

	UFUNCTION()
	void OnLeaveButtonClicked();


public:

	void NativeOnInitialized() override;

	UFUNCTION()
	void Config(EPlayerReadinessUIStatus PlayerReadiness);

	UFUNCTION(BlueprintNativeEvent)
	void CloseWidget();

	FVoidDataTransferSignature OnReadyButton;
	FVoidDataTransferSignature OnLeaveButton;
};
