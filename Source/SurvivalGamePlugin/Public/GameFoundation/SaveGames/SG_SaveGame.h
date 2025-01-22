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

	UPROPERTY(VisibleAnywhere, Category = "Cards | ResourceCards")
	TArray<FProgressiveDataHelperStruct> ResourceCards;

	UPROPERTY(EditDefaultsOnly, Category = "Cards | ResourceCards")
	uint8 MaxResourceCards = 100;

	UPROPERTY(VisibleAnywhere, Category = "Cards | HeroCards")
	TArray<FProgressiveDataHelperStruct> HeroCards;

	UPROPERTY(EditDefaultsOnly, Category = "Cards | HeroCards")
	uint8 MaxHeroCards = 50;

	UPROPERTY(VisibleAnywhere, Category = "Decks")
	TArray<FDeckPersistentData> Decks;

	UPROPERTY(VisibleAnywhere, Category = "Decks")
	uint8 ActiveDeckIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Decks")
	uint8 MaxDecks = 5;

	UFUNCTION()
	void Init();
};
