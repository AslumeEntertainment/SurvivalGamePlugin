// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/GameplayEffectDataObjects/GameplayEffectData.h"
#include "ConsumableItemEffectDataObject.generated.h"

class UGameplayEffect;

UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories)
class SURVIVALGAMEPLUGIN_API UConsumableItemEffectDataObject : public UGameplayEffectData
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	float EffectValue;

};
