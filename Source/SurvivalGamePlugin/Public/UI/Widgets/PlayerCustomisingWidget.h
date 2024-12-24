// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/GeneralDelegates.h"
#include "PlayerCustomisingWidget.generated.h"


class UTextButtonWidgetBase;
class UUnitIconWidget;
class UHorizontalBox;
class UWrapBox;
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
	UBorder* HeroCardBorder;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* MainHandCardsBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* SecondaryHandCardsBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextButtonWidgetBase* TXTBTN_ReadyStatus;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextButtonWidgetBase* TXTBTN_LeaveTable;

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UUnitIconWidget> HeroCardWidgetClass;

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UUnitIconWidget> ResourceCardWidgetClass;

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

	UFUNCTION()
	void Config(EPlayerReadinessUIStatus PlayerReadiness);

	void FillHeroCardSlot(FText Name, UTexture2D* Image, FLinearColor Color);

	void FillMainHandCardsSlots(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors);

	void FillSecondaryHandSlots(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors);

	UFUNCTION(BlueprintNativeEvent)
	void CloseWidget();

	FIntegerTransferSignature DeckSelected;
	FVoidDataTransferSignature OnReadyButton;
	FVoidDataTransferSignature OnLeaveButton;
};
