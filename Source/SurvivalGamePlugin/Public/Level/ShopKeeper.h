// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "ShopKeeper.generated.h"

class ASG_PlayerController;
class UShopWidgetBase;
class UShopNotificationWidgetBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API AShopKeeper : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopKeeper();

private:

	ASG_PlayerController* InteractingPlayer = nullptr;
	UPROPERTY()
	UShopWidgetBase* ShopWidget = nullptr;
	UPROPERTY()
	UShopNotificationWidgetBase* NotificationWidget = nullptr;

	TArray<FName> BoosterIDs;
	TArray<FText> BoosterNames;
	TArray<UTexture2D*> BoosterIcons;
	TArray<uint16> BoosterCosts;

	uint16 SelectedPackIndex = -1;

	UFUNCTION()
	void StopInteracting();

	UFUNCTION()
	void BoosterPackSelected(int Index);

	UFUNCTION()
	void BuyBoosterPack();

	UFUNCTION()
	void StartLootingAnimation(TArray<FDataTableRowHandle> Drops);

	UFUNCTION()
	void ShowNotification(bool CanAfford, FText Action, FText Object, uint16 Cost);

	UFUNCTION()
	void HideNotification();

	UFUNCTION()
	void UpdateWidgets();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UDataTable* BoosterPackTable;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UShopWidgetBase> ShopWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UShopNotificationWidgetBase> ShopNotificationWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	FText BuyingAction;

	UPROPERTY(EditAnywhere, Category = "UI")
	FText SellingAction;

	UPROPERTY(EditAnywhere, Category = "UI")
	uint16 MaxRNGRange = 10000;

public:	

	virtual void InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;
};
