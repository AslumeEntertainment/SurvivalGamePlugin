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

void AInGameHUD::SetInteractionOverlayBindings(ASG_Character* InteractingCharacter)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::SetInteractionOverlayBindings IsValid(World) == false"));
		return;
	}

	if (IsValid(InteractionOverlayWidget) == false)
	{
		InteractionOverlayWidget = CreateWidget<UInteractionOverlayWidget>(UGameplayStatics::GetGameInstance(World), InteractionOverlayWidgetClass);
	}
	
	InteractionOverlayWidget->SetInteractIndicationText(FText::FromString(""));
	if(IsValid(InteractingCharacter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::SetInteractionOverlayBindings IsValid(InteractingCharacter) == false"));
		return;
	}
	InteractingCharacter->InteractIndicationTextDelegate.AddDynamic(InteractionOverlayWidget, &UInteractionOverlayWidget::SetInteractIndicationText);
	InteractingCharacter->SettingsOpenedDelegate.AddDynamic(this, &AInGameHUD::ShowSettingsMenu);

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

void AInGameHUD::ShowGameplayOverlayWidget()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::ShowGameplayOverlayWidget IsValid(World) == false"));
		return;
	}
	GameplayOverlayWidget = CreateWidget<UInGameOverlayWidget>(UGameplayStatics::GetGameInstance(World), GameplayOverlayWidgetClass);
 
	AInGameCharacter* Player = Cast<AInGameCharacter>(PlayerOwner->GetPawn());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::ShowGameplayOverlayWidget IsValid(Player) == false"));
		return;
	}

	if (PlayerOwner && GameplayOverlayWidget)
	{
		Player->OnHealthPercentUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateHealthPercent);
		Player->OnHungerPercentUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateHungerPercent);
		Player->OnEnergyPercentUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateEnergyPercent);

		Player->OnMaxHealthUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::SetHealthBarLength);
		Player->OnMaxHungerUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::SetHungerBarLength);
		Player->OnMaxEnergyUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::SetEnergyBarLength);

		Player->OnCurrentSpeedUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateCurrentSpeed);

		GameplayOverlayWidget->AddToViewport();
		Player->BroadcastInitialValues();
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
	if (IsValid(SettingsMenu) == false)
	{
		SettingsMenu = CreateWidget<USettingsMenu>(PlayerOwner, SettingsMenuClass);
		SettingsMenu->CloseClicked.AddDynamic(this, &AInGameHUD::RemoveSettingsMenu);
	}

	if (bCanOpenMoreMenus == true && PlayerOwner && SettingsMenu && SettingsMenu->IsInViewport() == false)
	{
		bCanOpenMoreMenus = false;
		PlayerOwner->bShowMouseCursor = true;
		PlayerOwner->SetInputMode(FInputModeUIOnly());
		SettingsMenu->AddToViewport();
	}
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


