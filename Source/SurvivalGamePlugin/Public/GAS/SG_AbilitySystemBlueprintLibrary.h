// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SG_AbilitySystemBlueprintLibrary.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AbilitySystemBlueprintLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void CalculateAndApplyImpactDamage(AActor* Actor, const float Velocity);

};
