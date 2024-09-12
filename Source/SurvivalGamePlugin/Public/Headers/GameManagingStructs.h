#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameManagingStructs.generated.h"

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
struct FDeck
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int Hero = 0;

    UPROPERTY(EditAnywhere)
    TArray<int> StartingEquipment;

    UPROPERTY(EditAnywhere)
    TArray<int> SkillTree;
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

USTRUCT(BlueprintType)
struct FCampaignPersistentData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Code = "Name:2:s:1[d:0[]:1[d:::1-]:p:a:s:e-]";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CampaignIcon = nullptr;
    
};
