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

	UPROPERTY(EditAnywhere, Category = "Effects")
	int EffectLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> Effects;

public:

	FGameplayEffectSpecHandle CreateEffectSpecHandle(int EffectID);

	TArray<TSubclassOf<UGameplayEffect>> GetEffects();
};
