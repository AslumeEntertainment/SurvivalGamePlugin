// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "SGGA_ManageDeckInGame.generated.h"

class UAbilityTask_WaitInputRelease;
class UEquipmentComponent;
class APlayerController;
class AInGameCharacter;
class UUnitIconWidget;
class UInGameDeck;

UCLASS()
class SURVIVALGAMEPLUGIN_API USGGA_ManageDeckInGame : public USG_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


protected:

	AInGameCharacter* Player;
	APlayerController* PlayerCont;
	UEquipmentComponent* EquipmentComp;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInGameDeck> InGameDeckWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* MainHandResourceTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* SecondaryHandResourceTable = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* ItemsDataTable = nullptr;

	UPROPERTY()
	UInGameDeck* InGameDeckWidget;

	UPROPERTY()
	UAbilityTask_WaitInputRelease* WaitInputReleaseTask;

	UFUNCTION()
	void OpenDeckManager();

	UFUNCTION()
	void CloseDeckManager(float TimeHeld);

};
