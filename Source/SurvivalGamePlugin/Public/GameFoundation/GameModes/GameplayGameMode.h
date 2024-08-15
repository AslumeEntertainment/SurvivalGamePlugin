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

	bool LeaveTeam(AController* TriggeringPlayer, ETeam TeamToLeave);

	bool IsThereSpaceInTeam(ETeam TeamToCheck);
	void SpawnPlayerCustomiser(AController* TriggeringPlayer, ETeam TeamToSpawnTo);
	void DestroyPlayerCustomiser(AController* TriggeringPlayer, ETeam TeamToClean);

	UFUNCTION()
	void OnPlayerStatusUpdated();

protected:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldPrintDebug = true;

	UPROPERTY(EditAnywhere, Category = "TeamManagement")
	TMap<ETeam, FPlayerCustomiserSpawn> PerTeamCustomiserSpawnLocations;

	UPROPERTY(EditAnywhere, Category = "TeamManagement")
	TSubclassOf<APlayerCustomiser> PlayerCustomiserClassToSpawn;

	UPROPERTY(EditAnywhere, Category = "TeamManagement")
	TMap<ETeam, FTeamInfo> Teams;

	UPROPERTY(EditAnywhere, Category = "TeamManagement")
	TSubclassOf<ATeamManager> TeamManagerClassToSearch;


	void PostLogin(APlayerController* NewPlayer) override;

	void StartPlay() override;
};
