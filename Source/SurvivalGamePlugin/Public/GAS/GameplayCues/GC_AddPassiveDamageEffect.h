// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GC_AddPassiveDamageEffect.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API UGC_AddPassiveDamageEffect : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

protected:

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;

	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;
	
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;

};
