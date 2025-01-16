// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "DeckFunctionLibrary.generated.h"

class USG_SaveGame;
class UUnitIconWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API UDeckFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static const uint16 SeriesLenght = 10000;

	/**
	* Checks if given card is of given series.
	*
	* @param ID The ID of the card that will be checked.
	* @param Series The series that for wich will be check(Currently a series is long [SeriesLenght = 10000] IDs).
	* @return Whether the card is in the given series
	*/
	static bool IsCardFromSeries(uint16 ID, uint16 Series);


	/**
	* Searches for cards of specific series in the given save file.
	*
	* @param Save The save file buffer for which the search will take place.
	* @param DataTable The data table to use for collecting card info.
	* @param Series The series from wich cards will be collected. If left empty it will collect all cards regardless of series.
	* @return A map containing references to the found cards in the save file buffer, paired with the actual indexes of these cards.
	*/
	static TMap<uint16, FResourseCardPersistentData*> GetPlayerResourceSaveData(USG_SaveGame* Save, UDataTable* DataTable, TArray<uint16> Series);


	/**
	* Generates a new instance of a card with given ID.
	*
	* @param DataTable The data table to use for collecting card info.
	* @param ID The ID of the wanted card Type.
	* @return A instance of the card with the given ID
	*/
	static FResourseCardPersistentData GenerateCardWithID(UDataTable* DataTable, uint16 ID);

	static TArray<FHeroCardPersistentData*> GetPlayerHeroSaveData(USG_SaveGame* Save, UDataTable* HeroDataTable);
};
