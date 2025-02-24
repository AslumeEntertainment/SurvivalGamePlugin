// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "DamageEffect.generated.h"

UENUM(BlueprintType)
enum class ESG_DamageType : uint8
{
	None,
	Slashing,
	Piercing,
	Bludgeoning,
	Fire
};

UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories)
class SURVIVALGAMEPLUGIN_API UDamageEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	ESG_DamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	float BaseDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageEffectProperties")
	float Penetration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<UGameplayEffect>, float> AdditionalEffects;

};
