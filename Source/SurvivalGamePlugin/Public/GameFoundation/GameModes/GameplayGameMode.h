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

	UFUNCTION()
	bool UpdatePlayer(APlayerController* TriggeringPlayer /*, Soon add info of the player*/);

protected:

	UPROPERTY(EditAnywhere)
	TMap<ETeam, FTeamInfo> Teams;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATeamManager> TeamManagerClassToSearch;

	void PostLogin(APlayerController* NewPlayer) override;

	void StartPlay() override;
};
