// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Game/SettingsMenu.h"

#include "GameFramework/GameUserSettings.h"
#include "Components/Button.h"
#include "Components/Slider.h"


void USettingsMenu::UpdateSelectedGraphicsQuality(int GraphicsQuality)
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (IsValid(UserSettings) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USettingsMenu::UpdateSelectedGraphicsQuality IsValid(UserSettings) == false"));
		return;
	}

	if (GraphicsQuality != UserSettings->GetOverallScalabilityLevel())
	{
		UserSettings->SetOverallScalabilityLevel(GraphicsQuality);
		UserSettings->ApplyNonResolutionSettings();
		UserSettings->SaveSettings();
	}

	TArray<UButton*> ScalabilityButtons;
	ScalabilityButtons.Add(BTN_GraphicsLow);
	ScalabilityButtons.Add(BTN_GraphicsMedium);
	ScalabilityButtons.Add(BTN_GraphicsHigh);
	ScalabilityButtons.Add(BTN_GraphicsEpic);
	ScalabilityButtons.Add(BTN_GraphicsCinematic);

	for (int i = 0; i < 5; i++)
	{
		ScalabilityButtons[i]->SetIsEnabled(i == GraphicsQuality ? false : true);
	}
}

void USettingsMenu::OnLowClicked()
{
	GraphicsUpdated.Broadcast(0);
	UpdateSelectedGraphicsQuality(0);
}

void USettingsMenu::OnMediumClicked()
{
	GraphicsUpdated.Broadcast(1);
	UpdateSelectedGraphicsQuality(1);
}

void USettingsMenu::OnHighClicked()
{
	GraphicsUpdated.Broadcast(2);
	UpdateSelectedGraphicsQuality(2);
}

void USettingsMenu::OnEpicClicked()
{
	GraphicsUpdated.Broadcast(3);
	UpdateSelectedGraphicsQuality(3);
}

void USettingsMenu::OnCinematicClicked()
{
	GraphicsUpdated.Broadcast(4);
	UpdateSelectedGraphicsQuality(4);
}

void USettingsMenu::OnCloseClicked()
{
	CloseClicked.Broadcast();
}

void USettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BTN_GraphicsLow->OnClicked.AddDynamic(this, &USettingsMenu::OnLowClicked);
	BTN_GraphicsMedium->OnClicked.AddDynamic(this, &USettingsMenu::OnMediumClicked);
	BTN_GraphicsHigh->OnClicked.AddDynamic(this, &USettingsMenu::OnHighClicked);
	BTN_GraphicsEpic->OnClicked.AddDynamic(this, &USettingsMenu::OnEpicClicked);
	BTN_GraphicsCinematic->OnClicked.AddDynamic(this, &USettingsMenu::OnCinematicClicked);

	BTN_Close->OnClicked.AddDynamic(this, &USettingsMenu::OnCloseClicked);
}

void USettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	UpdateSelectedGraphicsQuality(IsValid(UserSettings) ? UserSettings->GetOverallScalabilityLevel() : -1);
}
