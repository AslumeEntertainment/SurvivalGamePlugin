// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "SG_GameplayAbilityTargetData.generated.h"


USTRUCT(BlueprintType)
struct SURVIVALGAMEPLUGIN_API FSG_GameplayAbilityTargetData : public FGameplayAbilityTargetData
{
    GENERATED_BODY()

    FSG_GameplayAbilityTargetData() 
    {
        ItemActor = nullptr;
    }

    UPROPERTY()
    AItemBase* ItemActor;

    virtual UScriptStruct* GetScriptStruct() const override
    {
        return FSG_GameplayAbilityTargetData::StaticStruct();
    }

    bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
    {
        Ar << ItemActor;
        bOutSuccess = true;
        return true;
    }
};

template <>
struct TStructOpsTypeTraits<FSG_GameplayAbilityTargetData> : public TStructOpsTypeTraitsBase2<FSG_GameplayAbilityTargetData>
{
    enum
    {
        WithNetSerializer = true
    };
};