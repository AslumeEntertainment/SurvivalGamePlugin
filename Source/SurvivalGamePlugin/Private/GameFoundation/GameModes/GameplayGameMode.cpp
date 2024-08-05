// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameModes/GameplayGameMode.h"

#include "Kismet/GameplayStatics.h" 

#include "GameFoundation/GameInstance/SG_GameInstance.h"
#include "Level/TeamManager.h"

DEFINE_LOG_CATEGORY_STATIC(TeamManagement, Log, All);

void AGameplayGameMode::StartPlay()
{

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay IsValid(World) == false"));
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, TeamManagerClassToSearch, FoundActors);

	ATeamManager* TeamManager = CastChecked<ATeamManager>(FoundActors[0]);

	TeamManager->OnPlayerChangingTeams.BindDynamic(this, &AGameplayGameMode::JoinTeam);

}

void AGameplayGameMode::JoinTeam(AController* TriggeringPlayer, ETeam TeamToJoin)
{
	ETeam CurrentTeam = GetCurrentTeam(TriggeringPlayer);

	if (LeaveTeam(TriggeringPlayer, CurrentTeam) == false)
	{
		UE_LOG(TeamManagement, Warning, TEXT("AGameplayGameMode::JoinTeam - Player could not leave current team: %s"), *UEnum::GetValueAsString(CurrentTeam));
		return;
	}

	FTeamInfo* TeamInfoPtr = Teams.Find(TeamToJoin);
	if (TeamInfoPtr == nullptr)
	{
		UE_LOG(TeamManagement, Error, TEXT("AGameplayGameMode::JoinTeam - No team associated with enum: %s, Check the GameMode settings"), *UEnum::GetValueAsString(TeamToJoin));
		return;
	}

	TeamInfoPtr->Players.Add(TriggeringPlayer);
}

ETeam AGameplayGameMode::GetCurrentTeam(AController* TriggeringPlayer)
{
	ETeam PlayerTeam = ETeam::Teamless;
	TArray<FTeamInfo> TeamArray;
	TArray<ETeam> EnumArray;
	Teams.GenerateValueArray(TeamArray);
	for (int i=0; i< TeamArray.Num() ; i++)
	{
		if (TeamArray[i].Players.Contains(TriggeringPlayer))
		{
			PlayerTeam = EnumArray[i];
			break;
		}
	}

	return PlayerTeam;
}

bool AGameplayGameMode::LeaveTeam(AController* TriggeringPlayer, ETeam TeamToLeave)
{
	FTeamInfo* TeamInfoPtr = Teams.Find(TeamToLeave);
	if (TeamInfoPtr == nullptr)
	{
		UE_LOG(TeamManagement, Error, TEXT("AGameplayGameMode::LeaveTeam - No team associated with enum: %s, Check the GameMode settings"), *UEnum::GetValueAsString(TeamToLeave));
		return false;
	}

	TeamInfoPtr->Players.Remove(TriggeringPlayer);

	return true;
}

bool AGameplayGameMode::UpdatePlayer(APlayerController* TriggeringPlayer)
{
	return true;
}

void AGameplayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::PostLogin IsValid(World) == false"));
		return;
	}

	USG_GameInstance* GameInstance = Cast<USG_GameInstance>(UGameplayStatics::GetGameInstance(World));
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::PostLogin IsValid(GameInstance) == false"));
		return;
	}

	GameInstance->RegisterNewPlayer(NewPlayer);

}




