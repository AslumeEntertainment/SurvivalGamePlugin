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

	AInGameCharacter* Player = Cast<AInGameCharacter>(PlayerOwner->GetPawn());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::BeginPlay IsValid(Player) == false"));
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::BeginPlay IsValid(World) == false"));
		return;
	}

	GameplayWidget = CreateWidget<UInGameOverlayWidget>(UGameplayStatics::GetGameInstance(World), GameplayWidgetClass);

	Player->OnHealthPercentUpdated.AddDynamic(GameplayWidget, &UInGameOverlayWidget::UpdateHealthPercent);
	Player->OnHungerPercentUpdated.AddDynamic(GameplayWidget, &UInGameOverlayWidget::UpdateHungerPercent);
	Player->OnEnergyPercentUpdated.AddDynamic(GameplayWidget, &UInGameOverlayWidget::UpdateEnergyPercent);

	Player->OnMaxHealthUpdated.AddDynamic(GameplayWidget, &UInGameOverlayWidget::SetHealthBarLength);
	Player->OnMaxHungerUpdated.AddDynamic(GameplayWidget, &UInGameOverlayWidget::SetHungerBarLength);
	Player->OnMaxEnergyUpdated.AddDynamic(GameplayWidget, &UInGameOverlayWidget::SetEnergyBarLength);

	ShowGameplayWidget();
}

void AInGameHUD::ShowGameplayWidget()
{
	if (PlayerOwner && GameplayWidget)
	{
		GameplayWidget->AddToViewport();
	}
}

