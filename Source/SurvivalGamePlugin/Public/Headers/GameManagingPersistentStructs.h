#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameManagingPersistentStructs.generated.h"

USTRUCT(BlueprintType)
struct FPersistentDataUnit : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FName ID;

    UPROPERTY(EditAnywhere)
    FText UnitName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* UnitImage = nullptr;

};

USTRUCT()
struct FBoosterPackHelperStruct
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FDataTableRowHandle RepresentedUnit;

    UPROPERTY(EditAnywhere)
    uint16 Chance = 25;
};

USTRUCT(BlueprintType)
struct FBoosterPackPersistentData : public FPersistentDataUnit
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    uint16 Cost = 10;

    UPROPERTY(EditAnywhere)
    uint8 NumberOfHeroes = 1;

    UPROPERTY(EditAnywhere)
    TArray<FBoosterPackHelperStruct> PossibleHeroes;

    UPROPERTY(EditAnywhere)
    uint8 NumberOfResources = 5;

    UPROPERTY(EditAnywhere)
    TArray<FBoosterPackHelperStruct> PossibleResources;

    UPROPERTY(EditAnywhere)
    uint8 NumberOfCampaigns = 1;

    UPROPERTY(EditAnywhere)
    TArray<FBoosterPackHelperStruct> PossibleCampaigns;

    UPROPERTY(EditAnywhere)
    uint8 NumberOfBiomes = 0;

    UPROPERTY(EditAnywhere)
    TArray<FBoosterPackHelperStruct> PossibleBiomes;

};

USTRUCT(BlueprintType)
struct FResourseCardPersistentData : public FPersistentDataUnit
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FDataTableRowHandle RepresentedItem;

    UPROPERTY(EditAnywhere)
    uint16 Value = 10;

};

USTRUCT(BlueprintType)
struct FHeroCardPersistentData : public FPersistentDataUnit
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TSubclassOf<ACharacter> HeroClass;

    UPROPERTY(EditAnywhere)
    TMap<uint8, uint16> ValuePerLevel;

};

USTRUCT(BlueprintType)
struct FBiomePersistentData : public FPersistentDataUnit
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Seed = "Idfk how to represent this shit";

};

USTRUCT(BlueprintType)
struct FCampaignPersistentData : public FPersistentDataUnit
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    uint8 DevPlayerNum = 2;

    UPROPERTY(EditAnywhere)
    FString Code = "::s:1[:d:0[]:1[d:::1~]:p:a:1:1-]";
    
};
