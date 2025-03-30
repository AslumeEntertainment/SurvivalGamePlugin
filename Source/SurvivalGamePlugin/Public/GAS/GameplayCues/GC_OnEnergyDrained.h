// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GC_OnEnergyDrained.generated.h"

class USGGA_OnEnergyDrained;

UCLASS()
class SURVIVALGAMEPLUGIN_API UGC_OnEnergyDrained : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
protected: 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	TSubclassOf<USGGA_OnEnergyDrained> OnEnergyDrainedPassiveAbility;

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;
};
