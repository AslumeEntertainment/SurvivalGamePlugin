// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionOverlayWidget.generated.h"

class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UInteractionOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_InteractIndicationText;

public:

	UFUNCTION()
	void SetInteractIndicationText(FText InteractIndicationText);

};
