// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitTooltipWidgetBase.generated.h"

class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UUnitTooltipWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_UnitName;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_UnitDescription;

public:

	UFUNCTION(BlueprintCallable)
	void SetUnitName(FString UnitName);

	UFUNCTION(BlueprintCallable)
	void SetUnitDescription(FString UnitDescription);

};
