// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicatorWidget.generated.h"

class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* DamageText;

public:

	void SetDamageText(float Damage);

	void SetFont(FSlateFontInfo FontInfo);
};
