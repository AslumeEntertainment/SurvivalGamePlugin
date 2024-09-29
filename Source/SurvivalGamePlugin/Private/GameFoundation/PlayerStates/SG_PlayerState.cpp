// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/PlayerStates/SG_PlayerState.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"

ASG_PlayerState::ASG_PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<USG_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<USG_CharacterAttributeSet>(TEXT("AttributeSet"));

	NetUpdateFrequency = 30.f;
}

UAbilitySystemComponent* ASG_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
