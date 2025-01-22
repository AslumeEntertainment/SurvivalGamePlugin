#pragma once

#include "CoreMinimal.h"
#include "DeckSegmentsEnum.generated.h"

UENUM(BlueprintType)
enum class EDeckSegment: uint8
{
	Hero,
	MainEquipment,
	SecondaryEquipment
};

