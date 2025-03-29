// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Items/ItemBase.h"
#include "ConsumableItem.generated.h"

class UConsumableItemEffectDataObject;

UCLASS()
class SURVIVALGAMEPLUGIN_API AConsumableItem : public AItemBase
{
	GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Effects")
    TArray<UConsumableItemEffectDataObject*> Effects;

public:

    TArray<UConsumableItemEffectDataObject*> GetEffects() { return Effects; };
	
};
