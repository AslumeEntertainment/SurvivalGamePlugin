// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoverUpWidgetBase.generated.h"

class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UCoverUpWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* AnnouncementBlock;

public:

	UFUNCTION()
	void SetAnnouncementTextFromString(FString AnnouncementString);
};
