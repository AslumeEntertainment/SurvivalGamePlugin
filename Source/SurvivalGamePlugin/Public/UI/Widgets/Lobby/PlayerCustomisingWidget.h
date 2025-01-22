// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "PlayerCustomisingWidget.generated.h"

class UTextButtonWidgetBase;
class UCardSelectorWidget;
class UHeroCardSelector;
class UUnitIconWidget;
class UHorizontalBox;
class UDeckSegmentPanel;
class UBorder;
class UButton;

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
	UHorizontalBox* DeckIconBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_CycleDecks_Left;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* BTN_CycleDecks_Right;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UDeckSegmentPanel* HeroCardBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UDeckSegmentPanel* MainHandCardsBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UDeckSegmentPanel* SecondaryHandCardsBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UBorder* CardSelectorBorder;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextButtonWidgetBase* TXTBTN_ReadyStatus;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextButtonWidgetBase* TXTBTN_LeaveTable;

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UHeroCardSelector> HeroCardWidgetClass;


	UFUNCTION()
	void SelectDeck(int DeckIndex);

	UFUNCTION()
	void CycleDecks_Left();

	UFUNCTION()
	void CycleDecks_Right();

	UFUNCTION()
	void OnReadyButtonClicked();

	UFUNCTION()
	void OnLeaveButtonClicked();


public:

	void NativeOnInitialized() override;

	UDeckSegmentPanel* GetHeroCardPanel();

	UDeckSegmentPanel* GetMainCardsPanel();

	UDeckSegmentPanel* GetSecondaryCardsPanel();

	void OpenCardSelector(UCardSelectorWidget* CardSelector);

	UFUNCTION()
	void Config(EPlayerReadinessUIStatus PlayerReadiness);

	UFUNCTION(BlueprintNativeEvent)
	void CloseWidget();

	FIntegerTransferSignature DeckSelected;
	FVoidDataTransferSignature OnReadyButton;
	FVoidDataTransferSignature OnLeaveButton;
};
