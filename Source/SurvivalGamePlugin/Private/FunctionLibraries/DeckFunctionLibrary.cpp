// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/DeckFunctionLibrary.h"

#include "GameFoundation/SaveGames/SG_SaveGame.h"

bool UDeckFunctionLibrary::IsCardFromSeries(uint16 ID, uint16 Series)
{
	return ID >= Series && ID<Series+ SeriesLenght;
}

TMap<uint16, FResourseCardPersistentData*> UDeckFunctionLibrary::GetPlayerResourceSaveData(USG_SaveGame* Save, UDataTable* DataTable, TArray<uint16> Series)
{
	TMap<uint16, FResourseCardPersistentData*> NeededResourceDataRows;

	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDeckFunctionLibrary::GetPlayerCardsSaveData IsValid(Save) == false"));
		return NeededResourceDataRows;
	}
	if (IsValid(DataTable) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDeckFunctionLibrary::GetPlayerCardsSaveData IsValid(DataTable) == false"));
		return NeededResourceDataRows;
	}

	uint16 ResourceIndex = -1;
	for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
	{
		ResourceIndex++;
		if (SavedResource.ID == 0) continue;

		bool bMustInclude = Series.IsEmpty();
		for (uint16 Serie : Series)
		{
			bMustInclude |= IsCardFromSeries(SavedResource.ID, Serie);
		}
		if (bMustInclude == false) continue;
	
		FResourseCardPersistentData* SavedResourceDataPtr = DataTable->FindRow<FResourseCardPersistentData>(FName(FString::FromInt(SavedResource.ID)), "Gathering Resources");
		if (SavedResourceDataPtr == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UDeckFunctionLibrary::GetPlayerCardsSaveData Resource[%d] doesn't exist in data table: %s"), SavedResource.ID, *DataTable->GetName());
			continue;
		}

		NeededResourceDataRows.Add(ResourceIndex, SavedResourceDataPtr);
	}

	return NeededResourceDataRows;
}

FResourseCardPersistentData UDeckFunctionLibrary::GenerateCardWithID(UDataTable* DataTable, uint16 ID)
{
	FResourseCardPersistentData* SavedResourceDataPtr = DataTable->FindRow<FResourseCardPersistentData>(FName(FString::FromInt(ID)), "Generating Card");
	if (SavedResourceDataPtr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDeckFunctionLibrary::GenerateCardWithID Card[%d] doesn't exist in data table: %s"), ID, *DataTable->GetName());
		return FResourseCardPersistentData();
	}
	return *SavedResourceDataPtr;
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
