// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStruct.generated.h"

UENUM(BlueprintType)
enum class EItemDisassembleRequirement : uint8
{
	Unable,
	Always,
	EmptyHand,
};

USTRUCT(BlueprintType)
struct FItemCraftingDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProductItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName IngredientItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMainItemDestroyed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsIngredientDestroyed = false;

};

USTRUCT(BlueprintType)
struct FItemDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	FItemDataStruct()
	{
		ItemName = FText::FromString("None");
		ItemImage = nullptr;
		ItemDurability = 0;
		ItemActorClass = nullptr;
		DisassembleRequirement = EItemDisassembleRequirement::Unable;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ItemDurability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FItemCraftingDataStruct> CraftableItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EItemDisassembleRequirement DisassembleRequirement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName MainParentItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName IngredientParentItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AItemBase> ItemActorClass;

	bool bCanCraftWithItem(FName IngredientItemName)
	{
		if (CraftableItems.IsEmpty())
		{
			return false;
		}

		for (FItemCraftingDataStruct CraftingRecipe : CraftableItems)
		{
			if (CraftingRecipe.IngredientItemID == IngredientItemName)
			{
				return true;
			}
		}

		return false;
	}

	FItemCraftingDataStruct GetCraftingRecipeByProductName(FName ProductName)
	{
		for (FItemCraftingDataStruct CraftingRecipe : CraftableItems)
		{
			if (CraftingRecipe.ProductItemID == ProductName)
			{
				return CraftingRecipe;
			}
		}
		return FItemCraftingDataStruct();
	}
};
