// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Core/UnitIconWidget.h"
#include "HeroCardSelector.generated.h"

class UImage;

UCLASS()
class SURVIVALGAMEPLUGIN_API UHeroCardSelector : public UUnitIconWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* IMG_HeroBackgroundImage;

public:

	UFUNCTION(BlueprintCallable)
	void SetHeroBackgroundImage(UTexture2D* ItemImage);
};
