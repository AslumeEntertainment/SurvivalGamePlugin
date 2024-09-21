#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameManagingStructs.generated.h"

class AItemBase;

UENUM(BlueprintType)
enum class ETeamDistribution : uint8
{
    AllTogether,
    EvenSplit,
    FreeForAll
};

USTRUCT(BlueprintType)
struct FDeck
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TSubclassOf<ACharacter> Hero;

    UPROPERTY(EditAnywhere)
    TArray<FName> StartingEquipment;
};
