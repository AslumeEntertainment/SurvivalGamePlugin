// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Stations/Station.h"

#include "Interfaces/InteractableInterface.h"
#include "ShopKeeper.generated.h"


class UShopWidgetBase;
class UWarningWidgetBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API AShopKeeper : public AStation
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopKeeper();

private:

	//can make a dlegate for getting the save game insted of this if we start optimizing

	UPROPERTY()
	UShopWidgetBase* ShopWidget = nullptr;
	UPROPERTY()
	UWarningWidgetBase* ShopWarningWidget = nullptr;

	UPROPERTY()
	TArray<uint16> BoosterIDs;
	UPROPERTY()
	TArray<FText> BoosterNames;
	UPROPERTY()
	TArray<UTexture2D*> BoosterIcons;
	UPROPERTY()
	TArray<FLinearColor> BoosterColors;
	UPROPERTY()
	TArray<uint16> BoosterCosts;

	UPROPERTY()
	TArray<FText> HeroNames;
	UPROPERTY()
	TArray<UTexture2D*> HeroIcons;
	UPROPERTY()
	TArray<FLinearColor> HeroColors;
	UPROPERTY()
	TArray<uint16> HeroCosts;
	UPROPERTY()
	TArray<uint16> HeroIndexes;

	UPROPERTY()
	TArray<FText> ResourceNames;
	UPROPERTY()
	TArray<UTexture2D*> ResourceIcons;
	UPROPERTY()
	TArray<FLinearColor> ResourceColors;
	UPROPERTY()
	TArray<uint16> ResourceCosts;
	UPROPERTY()
	TArray<uint16> ResourceIndexes;

	UPROPERTY()
	uint16 ActiveIndex = -1;

	UFUNCTION()
	void FillShopWidget();

	UFUNCTION()
	void BoosterPackSelected(int Index);

	UFUNCTION()
	void BuyBoosterPack();

	UFUNCTION()
	void HeroCardSelected(int Index);

	UFUNCTION()
	void SellHeroCard();

	UFUNCTION()
	void ResourceCardSelected(int Index);

	UFUNCTION()
	void SellResourceCard();

	UFUNCTION()
	void StartLootingAnimation(TArray<FDataTableRowHandle> Drops);

	UFUNCTION()
	void ShowNotification(FText Message, bool CanAfford);

	UFUNCTION()
	void HideNotification();

	UFUNCTION()
	void UpdateWidgets();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* BoosterPackTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* HeroTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* ResourceTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UShopWidgetBase> ShopWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UWarningWidgetBase> ShopWarningWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	FString WarningMessageBeginning;

	UPROPERTY(EditAnywhere, Category = "UI")
	FString BuyingAction;

	UPROPERTY(EditAnywhere, Category = "UI")
	FString SellingAction;

	UPROPERTY(EditAnywhere, Category = "UI")
	uint16 MaxRNGRange = 10000;

public:	
	virtual void StopInteracting() override;
	virtual bool InteractRequest(AController* InteractingCharacter) override;
	virtual FText GetInteractableName() override { return FText::FromString("Shopkeeper"); }
};
