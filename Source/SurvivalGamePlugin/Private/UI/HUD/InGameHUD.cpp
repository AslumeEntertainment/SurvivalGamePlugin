// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InGameHUD.h"

#include "Kismet/GameplayStatics.h"

#include "UI/Widgets/Core/InteractionOverlayWidget.h"
#include "UI/Widgets/Game/InGameOverlayWidget.h"
#include "UI/Widgets/Game/SettingsMenu.h"
#include "Characters/InGameCharacter.h"


void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerOwner)
	{
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		PlayerOwner->bShowMouseCursor = false;
	}
}

void AInGameHUD::SetCanOpenMoreMenus(bool NewValue)
{
	bCanOpenMoreMenus = NewValue;
}

void AInGameHUD::SetInteractionOverlayBindings(ASG_Character* InteractingCharacter)
{
	if (IsValid(InteractionOverlayWidget) == false)
	{
		InteractionOverlayWidget = CreateWidget<UInteractionOverlayWidget>(PlayerOwner, InteractionOverlayWidgetClass);
	}
	
	InteractionOverlayWidget->SetInteractIndicationText(FText::FromString(""));
	if(IsValid(InteractingCharacter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::SetInteractionOverlayBindings IsValid(InteractingCharacter) == false"));
		return;
	}

	if (InteractingCharacter->InteractIndicationTextDelegate.IsBound() == false)
	{
		InteractingCharacter->InteractIndicationTextDelegate.AddDynamic(InteractionOverlayWidget, &UInteractionOverlayWidget::SetInteractIndicationText);
	}
	
	if (InteractingCharacter->SettingsOpenedDelegate.IsBound() == false)
	{
		InteractingCharacter->SettingsOpenedDelegate.AddDynamic(this, &AInGameHUD::ShowSettingsMenu);
	}
	
	ShowInteractionOverlayWidget();
}

void AInGameHUD::ShowInteractionOverlayWidget()
{
	if (PlayerOwner && InteractionOverlayWidget && InteractionOverlayWidget->IsInViewport() == false)
	{
		InteractionOverlayWidget->AddToViewport();
	}
}

void AInGameHUD::RemoveInteractionOverlayWidget()
{
	if (PlayerOwner && InteractionOverlayWidget && InteractionOverlayWidget->IsInViewport() == true)
	{
		InteractionOverlayWidget->RemoveFromParent();
	}
}

void AInGameHUD::ShowGameplayOverlayWidget(AInGameCharacter* PlayerPawn)
{
	if (IsValid(PlayerPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::ShowGameplayOverlayWidget IsValid(PlayerPawn) == false"));
		return;
	}

	GameplayOverlayWidget = CreateWidget<UInGameOverlayWidget>(PlayerOwner, GameplayOverlayWidgetClass);
 
	/*AInGameCharacter* Player = PlayerOwner->GetPawn<AInGameCharacter>();
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::ShowGameplayOverlayWidget IsValid(Player) == false"));
		return;
	}*/

	if (PlayerOwner && GameplayOverlayWidget)
	{
		PlayerPawn->OnHealthPercentUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateHealthPercent);
		PlayerPawn->OnHungerPercentUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateHungerPercent);
		PlayerPawn->OnEnergyPercentUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateEnergyPercent);

		PlayerPawn->OnMaxHealthUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::SetHealthBarLength);
		PlayerPawn->OnMaxHungerUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::SetHungerBarLength);
		PlayerPawn->OnMaxEnergyUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::SetEnergyBarLength);

		PlayerPawn->OnCurrentSpeedUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateCurrentSpeed);

		GameplayOverlayWidget->AddToViewport();
		PlayerPawn->BroadcastInitialValues();
	}
}

void AInGameHUD::RemoveGameplayOverlayWidget()
{
	if (GameplayOverlayWidget && GameplayOverlayWidget->IsInViewport())
	{
		GameplayOverlayWidget->RemoveFromParent();
		GameplayOverlayWidget->Destruct();
	}
}

void AInGameHUD::ShowSettingsMenu()
{
	if (bCanOpenMoreMenus == false)
	{
		return;
	}

	if (IsValid(SettingsMenu) == false)
	{
		SettingsMenu = CreateWidget<USettingsMenu>(PlayerOwner, SettingsMenuClass);
		SettingsMenu->CloseClicked.AddDynamic(this, &AInGameHUD::RemoveSettingsMenu);
	}

	if (IsValid(PlayerOwner) == false || IsValid(SettingsMenu) == false)
	{
		return;
	}

	if (SettingsMenu->IsInViewport())
	{
		RemoveSettingsMenu();
	}
	
	bCanOpenMoreMenus = false;
	PlayerOwner->bShowMouseCursor = true;
	PlayerOwner->SetInputMode(FInputModeGameAndUI());
	SettingsMenu->AddToViewport();
}

void AInGameHUD::RemoveSettingsMenu()
{
	if (PlayerOwner && SettingsMenu && SettingsMenu->IsInViewport() == true)
	{
		bCanOpenMoreMenus = true;
		PlayerOwner->bShowMouseCursor = false;
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		SettingsMenu->RemoveFromParent();
	}
}

void AInGameHUD::ClearScreen()
{
	RemoveGameplayOverlayWidget();
	RemoveInteractionOverlayWidget();
	RemoveSettingsMenu();

	bCanOpenMoreMenus = true;
}


