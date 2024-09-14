#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameManagingPersistentStructs.generated.h"

USTRUCT(BlueprintType)
struct FBoosterPackPersistentData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FText PackName;

    UPROPERTY(EditAnywhere)
    uint16 Cost = 10;

    UPROPERTY(EditAnywhere)
    TMap<FName, uint16> PossibleHeroes;

    UPROPERTY(EditAnywhere)
    TMap<FName, uint16> PossibleResources;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* PackImage = nullptr;

};

USTRUCT(BlueprintType)
struct FResourseCardPersistentData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName RersentedItem = "00";

    UPROPERTY(EditAnywhere)
    uint16 Value = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CardImage = nullptr;

};

USTRUCT(BlueprintType)
struct FHeroCardPersistentData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TSubclassOf<ACharacter> HeroClass;

    UPROPERTY(EditAnywhere)
    TMap<uint8, uint16> ValuePerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CardImage = nullptr;

};

USTRUCT(BlueprintType)
struct FBiomePersistentData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Seed = "Idfk how to represent this shit";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* BiomeImage = nullptr;

};

USTRUCT(BlueprintType)
struct FCampaignPersistentData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Code = "Name:2:s:1[d:0[]:1[d:::1~]:p:a:s:e-]";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CampaignImage = nullptr;
    
};
