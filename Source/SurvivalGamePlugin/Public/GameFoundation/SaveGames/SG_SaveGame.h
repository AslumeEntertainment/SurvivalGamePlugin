// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "Headers/GameManagingPersistentStructs.h"

#include "SG_SaveGame.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_SaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, Category = "Account")
	uint32 Money = 0;

	UPROPERTY(VisibleAnywhere, Category = "Campaigns")
	TArray<FProgressiveDataHelperStruct> Campaigns;

	UPROPERTY(VisibleAnywhere, Category = "Campaigns")
	TArray<FName> Biomes;

	UPROPERTY(VisibleAnywhere, Category = "Cards")
	TArray<FProgressiveDataHelperStruct> ResourceCards;

	UPROPERTY(VisibleAnywhere, Category = "Cards")
	TArray<FProgressiveDataHelperStruct> HeroCards;

	UPROPERTY(VisibleAnywhere, Category = "Cards")
	TArray<FDeckPersistentData> Decks;

};
