// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/GameplayEffectData.h"
#include "KnockbackEffectData.generated.h"


UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories)
class SURVIVALGAMEPLUGIN_API UKnockbackEffectData : public UGameplayEffectData
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockbackEffectProperties")
	float KnockbackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockbackEffectProperties")
	bool bUseHitDirection = true;

};
