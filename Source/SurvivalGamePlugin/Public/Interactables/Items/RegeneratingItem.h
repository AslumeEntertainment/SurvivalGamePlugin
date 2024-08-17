// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Items/ItemBase.h"
#include "RegeneratingItem.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API ARegeneratingItem : public AItemBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Regeneration")
	float RegenerationAmount;

public:

	float GetRegenerationAmount();
};
