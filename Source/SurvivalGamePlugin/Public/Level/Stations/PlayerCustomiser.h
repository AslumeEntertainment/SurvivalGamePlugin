// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Stations/Station.h"

#include "Headers/GeneralDelegates.h"
#include "Headers/GameManagingStructs.h"

#include "PlayerCustomiser.generated.h"

class ASG_Character;
class AInGameCharacter;
class UPlayerCustomisingWidget;
class UCardSelectorWidget;
class UPanelWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCustomiserTransferSignature, APlayerCustomiser*, TriggeringPlayerCustomiser);

UCLASS()
class SURVIVALGAMEPLUGIN_API APlayerCustomiser : public AStation
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerCustomiser();

private:

	UPROPERTY()
	UPlayerCustomisingWidget* PlayerCustomisingWidget = nullptr;

	UPROPERTY(Replicated)
	bool bIsActive = false;

	UPROPERTY(Replicated, ReplicatedUsing = OnRepIsPlayerReady)
	bool bIsPlayerReady = false;

	UPROPERTY(Replicated)
	bool bIsTaken = false;

	UPROPERTY()
	ASG_Character* MainPawn = nullptr;

	UPROPERTY()
	AInGameCharacter* GamePawn = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_ToggleReady();

	UFUNCTION()
	void ToggleReady();

	UFUNCTION()
	void OpenCardSelector(UPanelWidget* CallerWidget);

	UFUNCTION()
	void DisplayDeckCards(uint8 NumCardsToDisplay, TArray<uint16> CardIndexes, UPanelWidget* CardBox);

	UFUNCTION(Server, Reliable)
	void Server_StopInteracting();

	UFUNCTION()
	void InGameCharacterDead();

protected:

	UPROPERTY(EditAnywhere, Category = "Decks")
	uint8 NumHeroesInDeck = 1;

	UPROPERTY(EditAnywhere, Category = "Decks")
	uint8 NumEquipmentInDeck = 6;

	UPROPERTY(EditAnywhere, Category = "Decks| Seriers")
	uint16 MainHandSeriesStart = 30000;

	UPROPERTY(EditAnywhere, Category = "Decks| Seriers")
	uint16 RuneStoneSeriesStart = 20000;

	UPROPERTY(EditAnywhere, Category = "Decks| Seriers")
	uint16 SecondaryHandSeriesStart = 10000;

	UPROPERTY(EditAnywhere, Category = "Decks| Seriers")
	uint16 GenericSeriesStart = 00000;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* HeroTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* MainHandResourceTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* SecondaryHandResourceTable = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AInGameCharacter> DefaultHeroClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPlayerCustomisingWidget> PlayerCustomisingWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UCardSelectorWidget> CardSelectorWidgetClass;

	UFUNCTION()
	void OnRepIsPlayerReady();

	UFUNCTION()
	void HandlePlayerLeavingSession(APlayerController* LeavingPlayer);

public:	

	virtual void BeginPlay() override;
	virtual bool InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;
	virtual void StopInteracting() override;
	virtual FText GetInteractableName() override { return FText::FromString("Join Table"); }

	UFUNCTION()
	void SetIsActive_Server(bool State);

	UFUNCTION()
	void UpdatePlayerReadiness(bool State);

	UFUNCTION()
	void SpawnHero(FVector Location, FRotator Rotation);

	UFUNCTION()
	void DespawnHero();

	UFUNCTION()
	void PossessMainPawn();

	UFUNCTION()
	void PossessGamePawn();

	UFUNCTION(Client, Reliable)
	void Client_SetInGameOverlayVisibility(bool bShouldAddToViewport);

	UFUNCTION(Client, Reliable)
	void Client_HideUI(bool bUsesAnimation = false);

	void HideUI(bool bUsesAnimation = false);

	UFUNCTION(Client, Reliable)
	void Client_ShowUI();

	void ShowUI();

	UFUNCTION(Client, Reliable)
	void Client_StopInteracting();

	FPlayerCustomiserTransferSignature OnPlayerStartInteracting;
	FPlayerCustomiserTransferSignature OnPlayerStopInteracting;
	FPlayerCustomiserTransferSignature OnPlayerReadinessChanged;
	FPlayerCustomiserTransferSignature OnInGameCharacterDeath;
};
