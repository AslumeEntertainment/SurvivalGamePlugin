// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Items/ItemBase.h"
#include "FluidContainerItem.generated.h"

class USG_TraceEmitterBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API AFluidContainerItem : public AItemBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "FluidContainerDefaults")
	USG_TraceEmitterBase* ScoopTrace;

public:

	UFUNCTION()
	void Scoop();
};
