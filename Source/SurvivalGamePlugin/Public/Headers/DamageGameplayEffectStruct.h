#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "DamageGameplayEffectStruct.generated.h"


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
struct FDamageGameplayEffect
{
	GENERATED_BODY()

	FDamageGameplayEffect()
	{
		DamageEffectClass = nullptr;
		DamageType = ESG_DamageType::None;
		DamageValue = 0.f;
		DamageEffectDuration = 0.f;
		DamageEffectPeriod = 0.f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SetByCallerMagnitudeDataTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESG_DamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageEffectDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageEffectPeriod;

};
