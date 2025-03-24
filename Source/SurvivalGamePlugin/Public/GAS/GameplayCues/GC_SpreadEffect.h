// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GC_SpreadEffect.generated.h"

class UGameplayEffectData;

UCLASS()
class SURVIVALGAMEPLUGIN_API UGC_SpreadEffect : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues", meta = (ToolTip = "The effect that will be spread to other actors. If not set, the gameplay cue will instead try to spread the effect that activated it"))
	UGameplayEffectData* GameplayEffectToSpread = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues", meta = (ClampMin = "1", ClampMax = "100", UIMin = "1", UIMax = "100"))
	int ChanceToSpread = 50;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	float ExtentFromTargetBounds = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	bool bShouldIgnoreSelf = true;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues")
	bool bShouldDrawDebug = true;

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;

};
