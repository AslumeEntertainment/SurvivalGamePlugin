// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalPropertyEnums.generated.h"

UENUM()
enum class EWeight : uint8
{
	Light,
	Normal,
	Heavy,
	Giant
};