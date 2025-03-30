// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "SpreadGameplayEffectComponent.generated.h"

class UGameplayEffectData;

UCLASS(DisplayName = "Spread Gameplay Effect while active")
class SURVIVALGAMEPLUGIN_API USpreadGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()
	
public:

	USpreadGameplayEffectComponent();

	virtual void OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const;

	UPROPERTY(EditAnywhere, Category = "CustomValues", meta = (ToolTip = "The effect that will be spread to other actors. If not set, the gameplay cue will instead try to spread the effect that activated it"))
	UGameplayEffectData* GameplayEffectToSpread = nullptr;

	UPROPERTY(EditAnywhere, Category = "CustomValues", meta = (ClampMin = "1", ClampMax = "100", UIMin = "1", UIMax = "100"))
	int ChanceToSpread = 50;

	UPROPERTY(EditAnywhere, Category = "CustomValues")
	float ExtentFromTargetBounds = 0.f;

	UPROPERTY(EditAnywhere, Category = "CustomValues")
	bool bShouldIgnoreSelf = true;

	UPROPERTY(EditAnywhere, Category = "CustomValues")
	bool bShouldDrawDebug = true;
};
