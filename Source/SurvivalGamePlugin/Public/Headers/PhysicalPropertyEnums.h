// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalPropertyEnums.generated.h"

UENUM(BlueprintType)
enum class EWeight : uint8
{
	None = 0,
	VeryLight = 1,
	Light = 5,
	Normal = 10,
	Heavy = 20,
	VeryHeavy = 30
};
ENUM_RANGE_BY_FIRST_AND_LAST(EWeight, EWeight::VeryLight, EWeight::VeryHeavy);