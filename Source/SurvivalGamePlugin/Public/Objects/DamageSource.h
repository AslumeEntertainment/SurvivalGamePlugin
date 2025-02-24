// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GAS/DamageEffect.h"
#include "GameplayTagContainer.h"

#include "DamageSource.generated.h"

class UGameplayEffect;

UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories)
class SURVIVALGAMEPLUGIN_API UDamageSource : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UDamageEffect*> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DamageSocket;
};
