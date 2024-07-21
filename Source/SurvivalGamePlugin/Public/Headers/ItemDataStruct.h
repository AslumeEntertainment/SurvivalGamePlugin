// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStruct.generated.h"


USTRUCT(BlueprintType)
struct FItemCraftingDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProductItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName IngredientItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsIngredientDestroyed = false;

};

USTRUCT(BlueprintType)
struct FItemDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ItemDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemCraftingDataStruct> CraftableItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItemBase> ItemActorClass;

	bool operator==(const FItemDataStruct& Item) const
	{
		if (ItemID == Item.ItemID) 
		{
			return true;
		}
		else return false;
	}

};
