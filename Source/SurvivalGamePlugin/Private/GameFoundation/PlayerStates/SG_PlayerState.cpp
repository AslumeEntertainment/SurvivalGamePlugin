// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/PlayerStates/SG_PlayerState.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"

ASG_PlayerState::ASG_PlayerState()
{
	NetUpdateFrequency = 30.f;
}

