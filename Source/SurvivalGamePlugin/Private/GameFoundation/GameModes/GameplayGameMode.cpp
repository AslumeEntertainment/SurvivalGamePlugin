// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameModes/GameplayGameMode.h"

#include "Kismet/GameplayStatics.h" 

#include "GameFoundation/GameInstance/SG_GameInstance.h"
#include "Level/TeamManager.h"
#include "Level/PlayerCustomiser.h"

DEFINE_LOG_CATEGORY_STATIC(TeamManagement, Log, All);

void AGameplayGameMode::StartPlay()
{
	Super::StartPlay();
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay IsValid(World) == false"));
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, TeamManagerClassToSearch, FoundActors);
	
	if (FoundActors.IsEmpty() == true)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay FoundActors.IsEmpty() == true - Please ensure there is an actor derived of class ATeamManager placed in the world"));
		return;
	}

	ATeamManager* TeamManager = Cast<ATeamManager>(FoundActors[0]);
	TeamManager->OnPlayerChangingTeams.BindDynamic(this, &AGameplayGameMode::JoinTeam);

}

void AGameplayGameMode::JoinTeam(AController* TriggeringPlayer, ETeam TeamToJoin)
{
	if(IsThereSpaceInTeam(TeamToJoin) == false)
	{
		UE_LOG(TeamManagement, Display, TEXT("AGameplayGameMode::JoinTeam - No free space in team %s"), *UEnum::GetValueAsString(TeamToJoin));
		return;
	}

	ETeam CurrentTeam = GetCurrentTeam(TriggeringPlayer);

	if (LeaveTeam(TriggeringPlayer, CurrentTeam) == false)
	{
		UE_LOG(TeamManagement, Warning, TEXT("AGameplayGameMode::JoinTeam - Player could not leave current team: %s"), *UEnum::GetValueAsString(CurrentTeam));
		return;
	}

	DestroyPlayerCustomiser(TriggeringPlayer, CurrentTeam);

	FTeamInfo* TeamInfoPtr = Teams.Find(TeamToJoin);
	if (TeamInfoPtr == nullptr)
	{
		UE_LOG(TeamManagement, Error, TEXT("AGameplayGameMode::JoinTeam - No team associated with enum: %s, Check the GameMode settings"), *UEnum::GetValueAsString(TeamToJoin));
		return;
	}

	TeamInfoPtr->Players.Add(TriggeringPlayer);
	
	SpawnPlayerCustomiser(TriggeringPlayer, TeamToJoin);

	UE_LOG(TeamManagement, Display, TEXT("Player(%s) Successfully joined Team: %s"), *TriggeringPlayer->GetName(), *UEnum::GetValueAsString(TeamToJoin));
}

bool AGameplayGameMode::IsThereSpaceInTeam(ETeam TeamToCheck)
{
	if (TeamToCheck == ETeam::Teamless)
	{
		return true;
	}

	FPlayerCustomiserSpawn* TeamPtr = PerTeamCustomiserSpawnLocations.Find(TeamToCheck);
	if (TeamPtr == nullptr)
	{
		UE_LOG(TeamManagement, Error, TEXT("AGameplayGameMode::DestroyPlayerCustomiser - No Spawnpoints associated with enum: %s, Check the GameMode settings"), *UEnum::GetValueAsString(TeamToCheck));
		return false;
	}

	TArray<APlayerCustomiser*> CustomiserArray;
	TeamPtr->Locations.GenerateValueArray(CustomiserArray);

	for (int i = 0; i < CustomiserArray.Num(); i++)
	{
		if (CustomiserArray[i] == nullptr) return true;
	}

	return false;
}

ETeam AGameplayGameMode::GetCurrentTeam(AController* TriggeringPlayer)
{
	ETeam PlayerTeam = ETeam::Teamless;
	TArray<FTeamInfo> TeamArray;
	TArray<ETeam> EnumArray;
	Teams.GenerateValueArray(TeamArray);
	Teams.GenerateKeyArray(EnumArray);
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

	if (!TeamInfoPtr->Players.Contains(TriggeringPlayer))
	{
		return true;
	}

	TeamInfoPtr->Players.Remove(TriggeringPlayer);
	UE_LOG(TeamManagement, Display, TEXT("Player(%s) Successfully left Team: %s"), *TriggeringPlayer->GetName(), *UEnum::GetValueAsString(TeamToLeave));
	return true;
}

void AGameplayGameMode::SpawnPlayerCustomiser(AController* TriggeringPlayer, ETeam TeamToSpawnTo)
{
	if (TeamToSpawnTo == ETeam::Teamless)
	{
		return;
	}

	FPlayerCustomiserSpawn* TeamPtr = PerTeamCustomiserSpawnLocations.Find(TeamToSpawnTo);
	if (TeamPtr == nullptr)
	{
		UE_LOG(TeamManagement, Error, TEXT("AGameplayGameMode::DestroyPlayerCustomiser - No Spawnpoints associated with enum: %s, Check the GameMode settings"), *UEnum::GetValueAsString(TeamToSpawnTo));
		return;
	}

	TArray<FVector> PositionArray;
	TArray<APlayerCustomiser*> CustomiserArray;
	TeamPtr->Locations.GenerateValueArray(CustomiserArray);
	TeamPtr->Locations.GenerateKeyArray(PositionArray);

	for (int i = 0; i < CustomiserArray.Num(); i++)
	{
		if (CustomiserArray[i] != nullptr) continue;

		UWorld* World = GetWorld();
		if (IsValid(World) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::SpawnPlayerCustomiser IsValid(World) == false"));
			return;
		}
		APlayerCustomiser* PlayerCustomiser = World->SpawnActor<APlayerCustomiser>(PlayerCustomiserClassToSpawn,PositionArray[i], TeamPtr->Rotations[i], FActorSpawnParameters());

		if (IsValid(PlayerCustomiser) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::SpawnPlayerCustomiser IsValid(PlayerCustomiser) == false"));
			return;
		}

		TeamPtr->Locations.Emplace(PositionArray[i], PlayerCustomiser);
		PlayerCustomiser->SetOwningPlayer(TriggeringPlayer);
		PlayerCustomiser->OnPlayerReadinessChanged.BindDynamic(this, &AGameplayGameMode::OnPlayerStatusUpdated);

		//Extract Player account info regarding pawns and populate the customiser with it

		return;
	}

}

void AGameplayGameMode::DestroyPlayerCustomiser(AController* TriggeringPlayer, ETeam TeamToClean)
{
	if (TeamToClean == ETeam::Teamless)
	{
		return;
	}

	FPlayerCustomiserSpawn* TeamPtr = PerTeamCustomiserSpawnLocations.Find(TeamToClean);
	if (TeamPtr == nullptr)
	{
		UE_LOG(TeamManagement, Error, TEXT("AGameplayGameMode::DestroyPlayerCustomiser - No Spawnpoints associated with enum: %s, Check the GameMode settings"), *UEnum::GetValueAsString(TeamToClean));
		return;
	}

	TArray<FVector> PositionArray;
	TArray<APlayerCustomiser*> CustomiserArray;
	TeamPtr->Locations.GenerateValueArray(CustomiserArray);
	TeamPtr->Locations.GenerateKeyArray(PositionArray);

	for (int i=0; i<CustomiserArray.Num();i++)
	{
		if (CustomiserArray[i] == nullptr) continue;

		if (CustomiserArray[i]->IsOwningPlayer(TriggeringPlayer))
		{
			CustomiserArray[i]->Destroy();
			TeamPtr->Locations.Emplace(PositionArray[i], nullptr);
			return;
		}
	}

}

void AGameplayGameMode::OnPlayerStatusUpdated()
{
	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("CHECKING READYNESS"));
	}

	//Run check on that if all players are ready
	TArray<FPlayerCustomiserSpawn> TeamArray;
	TArray<ETeam> EnumArray;
	PerTeamCustomiserSpawnLocations.GenerateValueArray(TeamArray);
	PerTeamCustomiserSpawnLocations.GenerateKeyArray(EnumArray);

	bool bGameShouldStart = true;

	for (int i=0 ; i<EnumArray.Num(); i++)
	{
		if (EnumArray[i] == ETeam::Teamless)
		{
			continue;
		}
		
		for (TPair<FVector, APlayerCustomiser*> CustomiserPair : TeamArray[i].Locations)
		{
			if (IsValid(CustomiserPair.Value) == false)
			{
				continue;
			}
			bGameShouldStart = bGameShouldStart && CustomiserPair.Value->GetIsPlayerReady();
		}

	}

	if (bGameShouldStart)
	{
		UE_LOG(LogTemp, Error, TEXT("GAME IS STARTING"));
	}
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

	NewPlayer->SetInputMode(FInputModeGameOnly());
}




