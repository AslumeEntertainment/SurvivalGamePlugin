// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"

#include "Objects/DamageSourceData.h"

#include "GC_ShowDamageIndicator.generated.h"

class ADamageIndicator;

UCLASS()
class SURVIVALGAMEPLUGIN_API UGC_ShowDamageIndicator : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	TSubclassOf<ADamageIndicator> DamageIndicatorClass;

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;
};
