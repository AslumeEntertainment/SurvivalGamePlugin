// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/GameplayEffectData.h"
#include "DamageSourceData.generated.h"

class UGameplayEffect;

DECLARE_LOG_CATEGORY_EXTERN(DamageLog, Log, All);

UENUM(BlueprintType)
enum class ESG_DamageType : uint8
{
	None,
	Slashing,
	Piercing,
	Bludgeoning,
	Fire
};

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
class SURVIVALGAMEPLUGIN_API UDamageSourceData : public UGameplayEffectData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	ESG_DamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	float BaseDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	float Penetration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	TArray<FAdditionalEffectDataStruct> AdditionalEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	bool bUseHitResultImpactLocation = true;
	
};
