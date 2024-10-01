// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_RegenerationAmount.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UMMC_RegenerationAmount : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:

	UMMC_RegenerationAmount();

	virtual float CalculateBaseMagnitude(const FGameplayEffectSpec& Spec) const;
};
