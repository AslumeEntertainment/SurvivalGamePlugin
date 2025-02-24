// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageDealerInterface.generated.h"

class UDamageDealingComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageDealerInterface : public UInterface
{
	GENERATED_BODY()
};


class SURVIVALGAMEPLUGIN_API IDamageDealerInterface
{
	GENERATED_BODY()

public:

	virtual void AddDamageSource(UDamageDealingComponent* NewDamageSource) = 0;

	virtual void RemoveDamageSourceByTag(FGameplayTag DamageSourceTag) = 0;

	virtual void SetDamageSourceActivenessByTag(FGameplayTag DamageSourceTag, bool ActivenessState) = 0;

	virtual void SetActivenessForAllDamageSources(bool ActivenessState) = 0;

};
