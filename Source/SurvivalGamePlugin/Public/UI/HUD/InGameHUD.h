// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Headers/ItemDataStruct.h"
#include "InGameHUD.generated.h"

class UInGameOverlayWidget;
class UItemIconWidget;
class UCraftingWindowWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay()override;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<UInGameOverlayWidget> GameplayWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<UItemIconWidget> ItemIconWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<UCraftingWindowWidget> CraftingWindowWidgetClass;

	UPROPERTY()
	UInGameOverlayWidget* GameplayWidget;

	UPROPERTY()
	UCraftingWindowWidget* CraftingWindowWidget;


public:
	
	void ShowGameplayWidget();

	UFUNCTION()
	void OpenCraftingWindow(FItemDataStruct MainItemData, FName IngredientItemName);

	UFUNCTION()
	void CloseCraftingWindow();
};
