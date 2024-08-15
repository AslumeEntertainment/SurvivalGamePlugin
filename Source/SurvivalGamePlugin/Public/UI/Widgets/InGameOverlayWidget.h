// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameOverlayWidget.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UInGameOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UBorder* HealthPercentOverlay;

public:

	UFUNCTION()
	void UpdateHealthPercent(float CurrentHealthPercent);
};
