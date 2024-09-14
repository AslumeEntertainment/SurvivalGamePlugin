// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "Headers/GameManagingStructs.h"

#include "SG_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API USG_SaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, Category = "Account")
	uint32 Money = 0;

	UPROPERTY(VisibleAnywhere, Category = "Campaigns")
	TMap<FName, uint8> Campaigns;

	UPROPERTY(VisibleAnywhere, Category = "Campaigns")
	TArray<FName> Biomes;

	UPROPERTY(VisibleAnywhere, Category = "Cards")
	TArray<FName> ResourseCards;

	UPROPERTY(VisibleAnywhere, Category = "Cards")
	TMap<FName, uint8> HeroCards;

	UPROPERTY(VisibleAnywhere, Category = "Cards")
	TArray<FDeck> Decks;

};
