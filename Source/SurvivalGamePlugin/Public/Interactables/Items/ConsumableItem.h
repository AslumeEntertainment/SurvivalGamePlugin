// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Items/ItemBase.h"
#include "ConsumableItem.generated.h"

class UConsumableItemEffectDataObject;
class UAIPerceptionStimuliSourceComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API AConsumableItem : public AItemBase
{
	GENERATED_BODY()

public:

    AConsumableItem();

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Effects")
    TArray<UConsumableItemEffectDataObject*> Effects;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UAIPerceptionStimuliSourceComponent* StimulusSource;

public:

    TArray<UConsumableItemEffectDataObject*> GetEffects() { return Effects; };
	
};
