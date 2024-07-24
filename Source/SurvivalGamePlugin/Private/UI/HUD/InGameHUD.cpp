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

	Player->CraftingWindowOpenDelegate.AddDynamic(this, &AInGameHUD::OpenCraftingWindow);
	Player->CraftingWindowCloseDelegate.AddDynamic(this, &AInGameHUD::CloseCraftingWindow);

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

void AInGameHUD::OpenCraftingWindow(FDataTableRowHandle MainItemRowData, FName IngredientItemName)
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
	FItemDataStruct* ItemData = MainItemRowData.GetRow<FItemDataStruct>(TEXT("AInGameHUD::ToggleCraftingWindow: Retrieving main hand item data"));
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameHUD::ToggleCraftingWindow ItemData == nullptr"));
		return;
	}

	for (FItemCraftingDataStruct CraftingRecipe : ItemData->CraftableItems)
	{
		if (CraftingRecipe.IngredientItemID == IngredientItemName)
		{
			FDataTableRowHandle CraftingRecipeRowData = MainItemRowData;
			CraftingRecipeRowData.RowName = CraftingRecipe.ProductItemID;

			FItemDataStruct* ProductItemData = CraftingRecipeRowData.GetRow<FItemDataStruct>(TEXT("AInGameHUD::ToggleCraftingWindow: Retrieving item data for product item"));
			if (!ProductItemData)
			{
				UE_LOG(LogTemp, Error, TEXT("AInGameHUD::ToggleCraftingWindow ProductItemData == nullptr"));
				return;
			}

			UItemIconWidget* ItemIcon = CreateWidget<UItemIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), ItemIconWidgetClass);
			ItemIcon->SetItemImage(ProductItemData->ItemImage);
			ItemIcon->SetItemName(ProductItemData->ItemName);
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
