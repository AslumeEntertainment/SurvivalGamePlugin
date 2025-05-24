// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Headers/ItemDataStruct.h"
#include "InGameHUD.generated.h"

class ASG_Character;
class AInGameCharacter;

class UInGameOverlayWidget;
class UInteractionOverlayWidget;
class USettingsMenu;

UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
	
private:

	bool bCanOpenMoreMenus = true;

protected:

	virtual void BeginPlay()override;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<UInGameOverlayWidget> GameplayOverlayWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<UInteractionOverlayWidget> InteractionOverlayWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<USettingsMenu> SettingsMenuClass;

	UPROPERTY()
	UInGameOverlayWidget* GameplayOverlayWidget;

	UPROPERTY()
	UInteractionOverlayWidget* InteractionOverlayWidget;

	UPROPERTY();
	USettingsMenu* SettingsMenu;

public:

	UFUNCTION(BlueprintCallable)
	void SetCanOpenMoreMenus(bool NewValue);

	UFUNCTION(BlueprintCallable)
	bool CanOpenMoreMenus() const { return bCanOpenMoreMenus; }

	UFUNCTION(BlueprintCallable)
	void SetInteractionOverlayBindings(ASG_Character* InteractingCharacter);

	UFUNCTION(BlueprintCallable)
	void ShowInteractionOverlayWidget();

	UFUNCTION(BlueprintCallable)
	void RemoveInteractionOverlayWidget();

	UFUNCTION(BlueprintCallable)
	void ShowGameplayOverlayWidget(AInGameCharacter* PlayerPawn);

	UFUNCTION(BlueprintCallable)
	void RemoveGameplayOverlayWidget();

	UFUNCTION(BlueprintCallable)
	void ShowSettingsMenu();

	UFUNCTION(BlueprintCallable)
	void RemoveSettingsMenu();

	UFUNCTION(BlueprintCallable)
	void ClearScreen();
};
