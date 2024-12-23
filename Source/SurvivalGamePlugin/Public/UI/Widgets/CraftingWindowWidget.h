// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingWindowWidget.generated.h"

class UWrapBox;
class UWidgetSwitcher;
class UUnitIconWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API UCraftingWindowWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* CraftingRecipeWrapBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

public:

	UFUNCTION()
	void AddCraftingRecipe(UUnitIconWidget* ItemIconWidget);

	UFUNCTION()
	void ClearRecipes();

	UFUNCTION()
	void SetWidgetSwitcherActiveIndex(int Index);
};
