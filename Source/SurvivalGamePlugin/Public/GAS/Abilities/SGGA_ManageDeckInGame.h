// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "SGGA_ManageDeckInGame.generated.h"

class UAbilityTask_WaitInputRelease;
class UEquipmentComponent;
class APlayerController;
class AInGameCharacter;
class UCardSlotWidget;
class UUnitIconWidget;
class UCardContainer;
class UInGameDeck;
class AItemBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_ManageDeckInGame : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


protected:

	bool bIsDropedItemToBeEquiped = false;

	AInGameCharacter* Player;
	APlayerController* PlayerCont;
	UEquipmentComponent* EquipmentComp;

	TArray<FResourseCardPersistentData*> EquipedItemsData;
	TArray<FResourseCardPersistentData*> MainItemsData;
	TArray<FResourseCardPersistentData*> SecondaryItemsData;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInGameDeck> InGameDeckWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* MainHandResourceTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* SecondaryHandResourceTable = nullptr;

	UPROPERTY()
	UInGameDeck* InGameDeckWidget;

	UPROPERTY()
	UAbilityTask_WaitInputRelease* WaitInputReleaseTask;

	UFUNCTION()
	void OpenDeckManager();

	UFUNCTION()
	void RefreshEquipedCards();

	UFUNCTION()
	void RefreshMainCards();

	UFUNCTION()
	void RefreshSecondaryCards();

	UFUNCTION()
	void CloseDeckManager(float TimeHeld);

	UFUNCTION()
	bool AttemptTransferCardFromDeckToWorld(UCardSlotWidget* Slot, int CardIndex);

	UFUNCTION()
	bool AttemptTransferCardFromDeckToEquipment(UCardSlotWidget* Slot, int CardIndex);

	UFUNCTION(BlueprintCallable)
	void SpawnItemFromCard(bool bShouldEquip, TSubclassOf<AItemBase> ItemClass, FItemDataStruct ItemData);

private:

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SpawnItemFromCard(bool bShouldEquip, TSubclassOf<AItemBase> ItemClass, FItemDataStruct ItemData);

	UFUNCTION()
	void EnableDroppingInWorld();
};
