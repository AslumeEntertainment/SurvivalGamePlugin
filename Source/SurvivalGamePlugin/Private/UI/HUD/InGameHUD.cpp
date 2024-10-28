// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InGameHUD.h"

#include "Kismet/GameplayStatics.h"

#include "UI/Widgets/InteractionOverlayWidget.h"
#include "UI/Widgets/InGameOverlayWidget.h"
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
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::BeginPlay IsValid(World) == false"));
		return;
	}
	ASG_Character* Player = Cast<ASG_Character>(PlayerOwner->GetPawn());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::BeginPlay IsValid(Player) == false"));
		return;
	}

	InteractionOverlayWidget = CreateWidget<UInteractionOverlayWidget>(UGameplayStatics::GetGameInstance(World), InteractionOverlayWidgetClass);
	Player->InteractIndicationTextDelegate.AddDynamic(InteractionOverlayWidget, &UInteractionOverlayWidget::SetInteractIndicationText);

	ShowInteractionOverlayWidget();
}

void AInGameHUD::ShowInteractionOverlayWidget()
{
	if (PlayerOwner && InteractionOverlayWidget)
	{
		InteractionOverlayWidget->AddToViewport();
	}
}

void AInGameHUD::ShowGameplayOverlayWidget()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::BeginPlay IsValid(World) == false"));
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


