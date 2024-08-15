// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingWindowWidget.generated.h"

class UWrapBox;
class UItemIconWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API UCraftingWindowWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* CraftingRecipeWrapBox;

public:

	UFUNCTION()
	void AddCraftingRecipe(UItemIconWidget* ItemIconWidget);

	UFUNCTION()
	void ClearRecipes();
};
