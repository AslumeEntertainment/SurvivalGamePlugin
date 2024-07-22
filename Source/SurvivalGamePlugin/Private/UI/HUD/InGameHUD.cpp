// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InGameHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/HumanCharacter.h"

#include "UI/Widgets/ItemIconWidget.h"
#include "UI/Widgets/CraftingWindowWidget.h"

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerOwner)
	{
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		PlayerOwner->bShowMouseCursor = false;
	}

	AHumanCharacter* Player = Cast<AHumanCharacter>(PlayerOwner->GetPawn());
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

	GameplayWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetGameInstance(World), GameplayWidgetClass);
	CraftingWindowWidget = CreateWidget<UCraftingWindowWidget>(UGameplayStatics::GetGameInstance(World), CraftingWindowWidgetClass);

	Player->CraftingWindowToggleDelegate.AddDynamic(this, &AInGameHUD::ToggleCraftingWindow);

	ShowGameplayWidget();
}

void AInGameHUD::ShowGameplayWidget()
{
	if (!PlayerOwner || !GameplayWidget)
	{
		return;
	}

	GameplayWidget->AddToViewport();
}

void AInGameHUD::ToggleCraftingWindow(TArray<FItemCraftingDataStruct> CraftingRecipes)
{
	if (!PlayerOwner || !CraftingWindowWidget)
	{
		return;
	}

	if (CraftingWindowWidget->IsInViewport())
	{
		CraftingWindowWidget->RemoveFromParent();
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		PlayerOwner->bShowMouseCursor = false;
	}
	else
	{
		CraftingWindowWidget->AddToViewport();
		PlayerOwner->SetInputMode(FInputModeUIOnly());
		PlayerOwner->bShowMouseCursor = true;
	}
}
