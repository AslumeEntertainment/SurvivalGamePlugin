// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Headers/TeamManagementStructs.h"

#include "GameplayGameMode.generated.h"

struct FTeamManagementInfo;
class APlayerController;

UCLASS()
class SURVIVALGAMEPLUGIN_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	bool JoinTeam(APlayerController* TriggeringPlayer, ETeam TeamToJoin);

	ETeam GetCurrentTeam(APlayerController* TriggeringPlayer);

	bool LeaveTeam(APlayerController* TriggeringPlayer, ETeam TeamToLeave);

	UFUNCTION()
	bool UpdatePlayer(APlayerController* TriggeringPlayer /*, Soon add info of the player*/);

protected:

	UPROPERTY(EditAnywhere)
	TMap<ETeam, FTeamInfo> Teams;

	void PostLogin(APlayerController* NewPlayer) override;
};
