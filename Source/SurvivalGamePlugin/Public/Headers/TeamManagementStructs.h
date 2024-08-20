#pragma once

#include "CoreMinimal.h"
#include "TeamManagementStructs.generated.h"

class APawn;
class APlayerController;
class APlayerCustomiser;

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Teamless,
	Prey,
	Survivor,
	Hunter,
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* PlayerCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* InGamePawn = nullptr;

	//Possible Pawn Data to choose from that changes depending on the team
};

USTRUCT(BlueprintType)
struct FTeamInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<AController*, FPlayerInfo> Players;

};
