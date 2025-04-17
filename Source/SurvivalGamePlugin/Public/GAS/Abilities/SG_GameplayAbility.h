// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SG_GameplayAbility.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	USG_GameplayAbility();

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (Categories = "AbilityType"))
	FGameplayTag AbilityTypeTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	FGameplayTag InputTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	bool bShouldActivateOnGranted = false;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
