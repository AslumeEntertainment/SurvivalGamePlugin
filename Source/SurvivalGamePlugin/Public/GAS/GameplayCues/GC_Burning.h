// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GC_Burning.generated.h"

class UNiagaraComponent;
class USceneComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API AGC_Burning : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:

	AGC_Burning();

protected:

	virtual void BeginPlay() override;

	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* FireNiagaraComponent = nullptr;

};
