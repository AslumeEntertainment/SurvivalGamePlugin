// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnnouncementWidgetBase.generated.h"

class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UAnnouncementWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* AnnouncementBlock;

public:

	UFUNCTION()
	void SetAnnouncementTextFromString(FString AnnouncementString);
};
