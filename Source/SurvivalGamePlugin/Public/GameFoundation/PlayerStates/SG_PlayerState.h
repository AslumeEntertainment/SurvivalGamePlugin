// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SG_PlayerState.generated.h"

class USG_AbilitySystemComponent;
class USG_CharacterAttributeSet;

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:

	ASG_PlayerState();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	USG_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	USG_CharacterAttributeSet* AttributeSet;

public:

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
