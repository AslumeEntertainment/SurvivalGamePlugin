// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "Interfaces/HittableInterface.h"
#include "HittableFoliageComponent.generated.h"

class ASG_Character;

UCLASS()
class SURVIVALGAMEPLUGIN_API UHittableFoliageComponent : public UFoliageInstancedStaticMeshComponent, public IHittableInterface
{
	GENERATED_BODY()
	
public:

	virtual void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles) override;

	virtual bool bCanBeHit() override;
};
