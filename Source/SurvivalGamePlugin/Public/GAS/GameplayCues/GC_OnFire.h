// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GC_OnFire.generated.h"

class USG_AbilitySystemComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API UGC_OnFire : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	UMaterialInterface* FireOverlay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	UNiagaraSystem* FireEffect;

	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;
};
