// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGameplayAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Action,
	Punch,
	Sprint,
	TestSwing,
	Disassemble,
	AnimalJump,
	AnimalBite,
	AnimalScratch
};
