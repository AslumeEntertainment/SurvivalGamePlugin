// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/DeckFunctionLibrary.h"

#include "GameFoundation/SaveGames/SG_SaveGame.h"

TArray<FResourseCardPersistentData*> UDeckFunctionLibrary::GetPlayerResourceSaveData(USG_SaveGame* Save, UDataTable* ResourceDataTable)
{
	TArray<FResourseCardPersistentData*> NeededResourceDataRows;

	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDeckFunctionLibrary::GetPlayerResourceSaveData IsValid(Save) == false"));
		return NeededResourceDataRows;
	}
	if (IsValid(ResourceDataTable) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDeckFunctionLibrary::GetPlayerResourceSaveData IsValid(ResourceDataTable) == false"));
		return NeededResourceDataRows;
	}

	TArray<FResourseCardPersistentData*> ResourceDataRows;
	
	ResourceDataTable->GetAllRows<FResourseCardPersistentData>("Filling resources", ResourceDataRows);
	for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
	{
		for (FResourseCardPersistentData* ResourceRow : ResourceDataRows)
		{
			if (SavedResource.ID != ResourceRow->ID) continue;

			NeededResourceDataRows.Add(ResourceRow);

			break;
		}
	}

	return NeededResourceDataRows;
}

TArray<FHeroCardPersistentData*> UDeckFunctionLibrary::GetPlayerHeroSaveData(USG_SaveGame* Save, UDataTable* HeroDataTable)
{
	TArray<FHeroCardPersistentData*> NeededHeroDataRows;

	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDeckFunctionLibrary::GetPlayerResourceSaveData IsValid(Save) == false"));
		return NeededHeroDataRows;
	}
	if (IsValid(HeroDataTable) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDeckFunctionLibrary::GetPlayerResourceSaveData IsValid(ResourceDataTable) == false"));
		return NeededHeroDataRows;
	}

	TArray<FHeroCardPersistentData*> HeroDataRows;
	
	HeroDataTable->GetAllRows<FHeroCardPersistentData>("Filling Heroes", HeroDataRows);
	for (FProgressiveDataHelperStruct HeroSaveData : Save->HeroCards)
	{
		for (FHeroCardPersistentData* HeroRow : HeroDataRows)
		{
			if (HeroSaveData.ID != HeroRow->ID) continue;

			NeededHeroDataRows.Add(HeroRow);

			break;
		}
	}

	return NeededHeroDataRows;
}
