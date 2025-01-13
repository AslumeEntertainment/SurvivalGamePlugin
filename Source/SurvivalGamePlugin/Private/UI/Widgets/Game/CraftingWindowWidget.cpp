// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Game/CraftingWindowWidget.h"

#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"

#include "UI/Widgets/Core/UnitIconWidget.h"

void UCraftingWindowWidget::AddCraftingRecipe(UUnitIconWidget* ItemIconWidget)
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
