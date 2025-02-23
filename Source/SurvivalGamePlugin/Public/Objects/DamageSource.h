// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GameplayTagContainer.h"
#include "Headers/DamageGameplayEffectStruct.h"

#include "DamageSource.generated.h"

class UGameplayEffect;

UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories)
class SURVIVALGAMEPLUGIN_API UDamageSource : public UObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY()
	float BaseDamageMultiplier = 1.f;

	UPROPERTY()
	float PenetrationMultiplier = 1.f;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DamageSourceTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESG_DamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Penetration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<UGameplayEffect>, float> AdditionalEffects;
	//TArray<FAdditionalDamageEffect> AdditionalEffects;

};
