// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Headers/TeamManagementStructs.h"

#include "GameplayGameMode.generated.h"

struct FTeamManagementInfo;
class APlayerController;
class ATeamManager;
class USG_GameInstance;
class APlayerCustomiser;
class ATimeEventManager;

USTRUCT(BlueprintType)
struct FPlayerCustomiserSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FVector, APlayerCustomiser*> Locations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRotator> Rotations;

};

UCLASS()
class SURVIVALGAMEPLUGIN_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:

	friend USG_GameInstance;

	UFUNCTION()
	void JoinTeam(AController* TriggeringPlayer, ETeam TeamToJoin);

	ETeam GetCurrentTeam(AController* TriggeringPlayer);

	bool IsThereSpaceInTeam(ETeam TeamToCheck);
	void SpawnPlayerCustomiser(AController* TriggeringPlayer, ETeam TeamToSpawnTo);
	void DestroyPlayerCustomiser(AController* TriggeringPlayer, ETeam TeamToClean);

	FVector GetValidInGameSpawnLocation();
	//void SpawnSurvivor();
	void SpawnPredator();

	UFUNCTION()
	void OnPlayerStatusUpdated();

protected:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldPrintDebug = true;

	UPROPERTY(EditAnywhere, Category = "CharacterSpawning")
	FVector BottomLeftArenaCornerCoordinate;

	UPROPERTY(EditAnywhere, Category = "CharacterSpawning")
	FVector UpperRightArenaCornerCoordinate;

	UPROPERTY(EditAnywhere, Category = "CharacterSpawning")
	float MinimalSpawnDistanceFromPlayer;

	UPROPERTY(EditAnywhere, Category = "TeamManagement")
	TMap<ETeam, FPlayerCustomiserSpawn> PerTeamCustomiserSpawnLocations;

	UPROPERTY(EditAnywhere, Category = "TeamManagement")
	TSubclassOf<APlayerCustomiser> PlayerCustomiserClassToSpawn;

	UPROPERTY(EditAnywhere, Category = "TeamManagement")
	TSubclassOf<ATeamManager> TeamManagerClassToSearch;

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	TSubclassOf<ATimeEventManager> TimeEventManagerClassToSearch;

	void PostLogin(APlayerController* NewPlayer) override;

	void StartPlay() override;
};
