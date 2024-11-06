// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Headers/DamageGameplayEffectStruct.h"
#include "PassiveDamageDealingComponent.generated.h"

class IHittableInterface;

UCLASS()
class SURVIVALGAMEPLUGIN_API UPassiveDamageDealingComponent : public UCapsuleComponent
{
	GENERATED_BODY()
	
public:

	UPassiveDamageDealingComponent();

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "DamageEffect")
	FDamageGameplayEffect DefaultDamageEffect;

	TArray<FGameplayEffectSpecHandle> InstantGameplayEffectSpecHandles;


public:

	//void ApplyInstantGameplayEffectSpecHandle(FGameplayEffectSpecHandle SpecHandle, bool bShouldApplyOnOverlap = false);

};
