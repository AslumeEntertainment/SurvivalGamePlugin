#pragma once

#include "CoreMinimal.h"
#include "TeamManagementStructs.generated.h"

class APawn;
class APlayerController;

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Teamless,
	Prey,
	Survivor,
	Hunter,
};

USTRUCT(BlueprintType)
struct FPlayerCharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* PlayerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> InGamePawnClass;

};

USTRUCT(BlueprintType)
struct FTeamInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<APlayerController*, FPlayerCharacterInfo> Players;

};
