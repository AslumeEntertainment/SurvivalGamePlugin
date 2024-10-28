// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Headers/ItemDataStruct.h"
#include "InGameHUD.generated.h"

class UInGameOverlayWidget;
class UInteractionOverlayWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay()override;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<UInGameOverlayWidget> GameplayOverlayWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<UInteractionOverlayWidget> InteractionOverlayWidgetClass;

	UPROPERTY()
	UInGameOverlayWidget* GameplayOverlayWidget;

	UPROPERTY()
	UInteractionOverlayWidget* InteractionOverlayWidget;

public:

	UFUNCTION(BlueprintCallable)
	void ShowInteractionOverlayWidget();

	UFUNCTION(BlueprintCallable)
	void ShowGameplayOverlayWidget();

	UFUNCTION(BlueprintCallable)
	void RemoveGameplayOverlayWidget();


};
