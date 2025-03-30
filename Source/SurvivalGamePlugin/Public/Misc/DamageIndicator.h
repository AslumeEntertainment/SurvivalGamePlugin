// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Objects/GameplayEffectDataObjects/DamageSourceData.h"

#include "DamageIndicator.generated.h"

class UWidgetComponent;
class UProjectileMovementComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API ADamageIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ADamageIndicator();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	TMap<ESG_DamageType, FSlateFontInfo> DamageIndicatorFontForDamageType;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComponent;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void DestroyDamageIndicator();

public:

	UPROPERTY(Replicated)
	float Damage = 0.f;

	UPROPERTY()
	ESG_DamageType DamageType;

};
