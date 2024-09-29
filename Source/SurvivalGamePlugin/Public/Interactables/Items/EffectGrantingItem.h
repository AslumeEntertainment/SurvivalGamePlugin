// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Items/ItemBase.h"
#include "EffectGrantingItem.generated.h"

class UGameplayEffect;

UCLASS()
class SURVIVALGAMEPLUGIN_API AEffectGrantingItem : public AItemBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TMap<TSubclassOf<UGameplayEffect>, uint8> Effects;

public:

	TMap<TSubclassOf<UGameplayEffect>, uint8> GetEffects();
};
