// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InGameHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/HumanCharacter.h"

#include "UI/Widgets/InGameOverlayWidget.h"
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

	ASG_Character* Player = Cast<ASG_Character>(PlayerOwner->GetPawn());
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
	CraftingWindowWidget = CreateWidget<UCraftingWindowWidget>(UGameplayStatics::GetGameInstance(World), CraftingWindowWidgetClass);

	Player->OnHealthPercentUpdated.AddDynamic(GameplayWidget, &UInGameOverlayWidget::UpdateHealthPercent);
	if (Player->OnHealthPercentUpdated.IsBound())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, TEXT("HUD Player->OnHealthPercentUpdated.IsBound() == true"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, TEXT("HUD Player->OnHealthPercentUpdated.IsBound() == false"));
	}

	if (Cast<AHumanCharacter>(Player))
	{
		Cast<AHumanCharacter>(Player)->CraftingWindowOpenDelegate.AddDynamic(this, &AInGameHUD::OpenCraftingWindow);
		Cast<AHumanCharacter>(Player)->CraftingWindowCloseDelegate.AddDynamic(this, &AInGameHUD::CloseCraftingWindow);
	}

	ShowGameplayWidget();
	GameplayWidget->UpdateHealthPercent(1);
}

void AInGameHUD::ShowGameplayWidget()
{
	if (!PlayerOwner || !GameplayWidget)
	{
		return;
	}

	GameplayWidget->AddToViewport();
}

void AInGameHUD::OpenCraftingWindow(FItemDataStruct MainItemData, FName IngredientItemName)
{
	if (!PlayerOwner || !CraftingWindowWidget)
	{
		return;
	}

	AHumanCharacter* Player = Cast<AHumanCharacter>(PlayerOwner->GetPawn());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::ToggleCraftingWindow IsValid(Player) == false"));
		return;
	}

	for (FItemCraftingDataStruct CraftingRecipe : MainItemData.CraftableItems)
	{
		if (CraftingRecipe.IngredientItemID == IngredientItemName)
		{
			UItemIconWidget* ItemIcon = CreateWidget<UItemIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), ItemIconWidgetClass);
			ItemIcon->SetItemImage(CraftingRecipe.ProductItemImage);
			ItemIcon->SetItemName(CraftingRecipe.ProductItemName);
			ItemIcon->IconClicked.AddDynamic(Player, &AHumanCharacter::CraftItem);
			CraftingWindowWidget->AddCraftingRecipe(ItemIcon);
		}
	}

	Player->SwitchInputMappingContext(EInputMappingContextState::Crafting);
	CraftingWindowWidget->AddToViewport();
	PlayerOwner->SetInputMode(FInputModeGameAndUI());
	PlayerOwner->bShowMouseCursor = true;
}

void AInGameHUD::CloseCraftingWindow()
{
	if (!PlayerOwner || !CraftingWindowWidget)
	{
		return;
	}
	AHumanCharacter* Player = Cast<AHumanCharacter>(PlayerOwner->GetPawn());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::CloseCraftingWindow IsValid(Player) == false"));
		return;
	}

	Player->SwitchInputMappingContext(EInputMappingContextState::Default);
	CraftingWindowWidget->RemoveFromParent();
	CraftingWindowWidget->ClearRecipes();
	PlayerOwner->SetInputMode(FInputModeGameOnly());
	PlayerOwner->bShowMouseCursor = false;
}
