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

	//UFUNCTION(BlueprintCallable)
	static TArray<FResourseCardPersistentData*> GetPlayerResourceSaveData(USG_SaveGame* Save, UDataTable* ResourceDataTable);

	//UFUNCTION(BlueprintCallable)
	static TArray<FHeroCardPersistentData*> GetPlayerHeroSaveData(USG_SaveGame* Save, UDataTable* HeroDataTable);

};
