#pragma once

#include "CoreMinimal.h"
#include "SessionState.generated.h"

UENUM(BlueprintType)
enum class ESessionState : uint8
{
	Hosting,
	Joined,
	Idle,
};
