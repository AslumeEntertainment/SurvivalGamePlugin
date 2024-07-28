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
	FText ProductItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* ProductItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName IngredientItemID;

};

USTRUCT(BlueprintType)
struct FItemDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	FItemDataStruct()
	{
		ItemName = FText::FromString("None");
		ItemImage = nullptr;
		ItemDurability = 0.f;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ItemDurability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FItemCraftingDataStruct> CraftableItems;

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
	/*
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
	*/
};


USTRUCT(BlueprintType)
struct FCraftingProcessDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	FCraftingProcessDataStruct()
	{
		ProcessLength = 0.f;
		Animation = nullptr;
		MovementSpeedMultiplier = 0.f;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ProcessLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* Animation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MovementSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FName> ItemsToDestroy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AItemBase> ItemActorClass;

};


USTRUCT(BlueprintType)
struct FDisassemblyProcessDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	FDisassemblyProcessDataStruct()
	{
		ProcessLength = 0.f;
		Animation = nullptr;
		MovementSpeedMultiplier = 0.f;
		DisassembleRequirement = EItemDisassembleRequirement::Unable;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ProcessLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* Animation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MovementSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EItemDisassembleRequirement DisassembleRequirement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AItemBase> MainParentItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AItemBase> IngredientParentItem;
};
