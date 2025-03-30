// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayEffectData.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAdditionalEffectDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UGameplayEffectData* AdditionalEffectData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float ChanceToApply = 0.f;

};

UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories)
class SURVIVALGAMEPLUGIN_API UGameplayEffectData : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	float EffectLevel = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	TArray<FAdditionalEffectDataStruct> AdditionalEffects;

};
