// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Headers/GeneralDelegates.h"

#include "SettingsMenu.generated.h"

class UButton;
class USlider;

UCLASS()
class SURVIVALGAMEPLUGIN_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()
	
private:

	void UpdateSelectedGraphicsQuality(int GraphicsQuality);

protected:

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (BindWidget))
	UButton* BTN_GraphicsLow;

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (BindWidget))
	UButton* BTN_GraphicsMedium;

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (BindWidget))
	UButton* BTN_GraphicsHigh;

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (BindWidget))
	UButton* BTN_GraphicsEpic;

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (BindWidget))
	UButton* BTN_GraphicsCinematic;

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (BindWidget))
	USlider* SoundSlider;

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (BindWidget))
	UButton* BTN_Close;

	UFUNCTION()
	void OnLowClicked();

	UFUNCTION()
	void OnMediumClicked();

	UFUNCTION()
	void OnHighClicked();

	UFUNCTION()
	void OnEpicClicked();

	UFUNCTION()
	void OnCinematicClicked();

	UFUNCTION()
	void OnCloseClicked();

public:

	void NativeOnInitialized() override;

	void NativeConstruct() override;

	FIntegerTransferSignature GraphicsUpdated;
	FVoidDataTransferSignature CloseClicked;
};
