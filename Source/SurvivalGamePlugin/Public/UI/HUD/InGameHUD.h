// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Headers/ItemDataStruct.h"
#include "InGameHUD.generated.h"

class UInGameOverlayWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay()override;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClasses")
	TSubclassOf<UInGameOverlayWidget> GameplayOverlayWidgetClass;

	UPROPERTY()
	UInGameOverlayWidget* GameplayOverlayWidget;

public:
	
	void ShowGameplayOverlayWidget();

	void RemoveGameplayOverlayWidget(AActor* OwningPawn);

};
