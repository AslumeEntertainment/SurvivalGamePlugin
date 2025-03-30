// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GC_SpawnNiagaraSystemOnTarget.generated.h"

class UNiagaraSystem;

UCLASS()
class SURVIVALGAMEPLUGIN_API UGC_SpawnNiagaraSystemOnTarget : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	bool bUseLocationParameter = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	UNiagaraSystem* NiagaraSystem;

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;
};
