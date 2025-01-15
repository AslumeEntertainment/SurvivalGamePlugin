// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Headers/TeamManagementStructs.h"
#include "Headers/generalDelegates.h"

#include "GameplayGameMode.generated.h"

struct FTeamManagementInfo;
class APlayerController;
class ATeamManager;
class USG_GameInstance;
class ASG_PlayerController;
class APlayerCustomiser;
class ATimeEventManager;


UCLASS()
class SURVIVALGAMEPLUGIN_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:

	friend USG_GameInstance;

	UFUNCTION()
	void OnPlayerStatusUpdated();

	FVector GetValidInGameSpawnLocation();

	void PrepareGame();

	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void EndGame();

	//UFUNCTION()
	//void OnPlayerReturningToLobby(APlayerCustomiser* TriggeringPlayerCustomiser);

	UFUNCTION()
	void KickAllPlayers();

	UFUNCTION()
	void UnRegisterRequest(APlayerController* TriggeringPlayer);

	
	FTimerHandle LootingPeriodTimer;

	TMap<FString, ASG_PlayerController*> AllPlayers;

	FPlayerControllerTransferSignature OnRegisterNeeded;
	FPlayerControllerTransferSignature OnUnregisterNeeded;

protected:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldPrintDebug = true;

	UPROPERTY(EditAnywhere, Category = "CharacterSpawning")
	FVector BottomLeftArenaCornerCoordinate;

	UPROPERTY(EditAnywhere, Category = "CharacterSpawning")
	FVector UpperRightArenaCornerCoordinate;

	UPROPERTY(EditAnywhere, Category = "CharacterSpawning")
	float MinimalSpawnDistanceFromPlayer;

	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;
	void SendPlayerListUpdate();
	void StartPlay() override;

	UFUNCTION(BlueprintCallable)
	void KickPlayer(FString PlayerName);

	FStringArrayTransferSignature OnPlayerListUpdated;
	
};
