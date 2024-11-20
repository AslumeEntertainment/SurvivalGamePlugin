// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/CraftingWindowWidget.h"

#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"

#include "UI/Widgets//ItemIconWidget.h"

void UCraftingWindowWidget::AddCraftingRecipe(UItemIconWidget* ItemIconWidget)
{
	CraftingRecipeWrapBox->AddChildToWrapBox(ItemIconWidget);
}

void UCraftingWindowWidget::ClearRecipes()
{
	if (CraftingRecipeWrapBox->HasAnyChildren())
	{
		CraftingRecipeWrapBox->ClearChildren();
	}
}

void UCraftingWindowWidget::SetWidgetSwitcherActiveIndex(int Index)
{
	WidgetSwitcher->SetActiveWidgetIndex(Index);
}
