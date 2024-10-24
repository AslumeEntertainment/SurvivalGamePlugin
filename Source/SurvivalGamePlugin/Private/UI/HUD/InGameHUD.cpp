// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InGameHUD.h"

#include "Kismet/GameplayStatics.h"

#include "UI/Widgets/CraftingWindowWidget.h"
#include "UI/Widgets/InGameOverlayWidget.h"
#include "UI/Widgets/ItemIconWidget.h"
#include "Characters/InGameCharacter.h"


void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerOwner)
	{
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		PlayerOwner->bShowMouseCursor = false;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::BeginPlay IsValid(World) == false"));
		return;
	}

	GameplayOverlayWidget = CreateWidget<UInGameOverlayWidget>(UGameplayStatics::GetGameInstance(World), GameplayOverlayWidgetClass);
}

void AInGameHUD::ShowGameplayOverlayWidget()
{
	AInGameCharacter* Player = Cast<AInGameCharacter>(PlayerOwner->GetPawn());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::BeginPlay IsValid(Player) == false"));
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

		Player->InteractIndicationTextDelegate.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::SetInteractIndicationText);
		Player->OnCurrentSpeedUpdated.AddDynamic(GameplayOverlayWidget, &UInGameOverlayWidget::UpdateCurrentSpeed);

		GameplayOverlayWidget->AddToViewport();
	}
}

void AInGameHUD::RemoveGameplayOverlayWidget(AActor* OwningPawn)
{
	if (GameplayOverlayWidget && GameplayOverlayWidget->IsInViewport())
	{
		GameplayOverlayWidget->RemoveFromParent();
	}
}


