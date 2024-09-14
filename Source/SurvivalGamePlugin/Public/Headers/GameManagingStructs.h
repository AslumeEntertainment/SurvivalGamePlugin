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
struct FRuleset
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    bool bCanUseSkillTree = false;

    UPROPERTY(EditAnywhere)
    bool bCanUseStartingEquipment = false;

    UPROPERTY(EditAnywhere)
    ETeamDistribution TeamDistribution = ETeamDistribution::AllTogether;

    UPROPERTY(EditAnywhere)
    int ProgressionType = 1;
};

USTRUCT(BlueprintType)
struct FCardPersistentData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName RepresentedItem;

    UPROPERTY(EditAnywhere)
    uint16 Value = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CardImage = nullptr;

};

USTRUCT(BlueprintType)
struct FDeck
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    uint8 Hero = 0;

    UPROPERTY(EditAnywhere)
    TArray<uint16> StartingEquipment;
};

USTRUCT(BlueprintType)
struct FGame
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int Biome = 0;

    UPROPERTY(EditAnywhere)
    TArray<int> GuaranteedStructures;

    UPROPERTY(EditAnywhere)
    TArray<FDeck> Enemies;

    UPROPERTY(EditAnywhere)
    TArray<int> SpawnTimes;

    FRuleset RuleSet;
};

USTRUCT(BlueprintType)
struct FCampaign
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Name = "Campaign";

    UPROPERTY(EditAnywhere)
    int NumPlayers = 1;

    UPROPERTY(EditAnywhere)
    bool IsSuperVised = false;

    UPROPERTY(EditAnywhere)
    TArray<FGame> Games;
};