// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameModes/GameplayGameMode.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"
#include "Level/TeamManager.h"
//#include "Level/PlayerCustomiser.h"
#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(TeamManagement, Log, All);

void AGameplayGameMode::StartPlay()
{
	UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay"));

	Super::StartPlay();

	USessionSubsystem* SessionSubsystemPtr = GetGameInstance()->GetSubsystem<USessionSubsystem>();
	if (IsValid(SessionSubsystemPtr) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay (IsValid(SessionSubsystem) == false"));
		return;
	}

	SessionSubsystemPtr->OnHostedGameClosing.AddDynamic(this, &AGameplayGameMode::KickAllPlayers);
	OnRegisterNeeded.AddDynamic(SessionSubsystemPtr, &USessionSubsystem::RegisterNewPlayer);
	OnUnregisterNeeded.AddDynamic(SessionSubsystemPtr, &USessionSubsystem::UnRegisterPlayer);
}

void AGameplayGameMode::KickAllPlayers()
{
	for (TPair<FString,ASG_PlayerController*> Player : AllPlayers)
	{
		if (Player.Value->IsLocalController())
		{
			continue;
		}

		if (bShouldPrintDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("KICKING: %s"), *Player.Key);
		}

		Player.Value->Client_LeaveSession(true);
	}
}

void AGameplayGameMode::OnPlayerStatusUpdated()
{
	/*
	TO BE REWORKED
	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("CHECKING READYNESS"));
	}

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

	if (!bGameShouldStart)
	{
		return;
	}
	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Error, TEXT("GAME WILL START IN %f SECONDS"), LoadingPeriodLenght);
	}
	*/

	PrepareGame();

}

void AGameplayGameMode::PrepareGame()
{
	//GetWorldTimerManager().SetTimer(LoadingPeriodTimer, this, &AGameplayGameMode::StartGame, 1, true, LoadingPeriodLenght);

	//Start Generating map

	//SpawnSurvivors();
}

void AGameplayGameMode::StartGame()
{
	/*
	* TO BE REWORKED
	if (false)//check if map is already loaded propperly
	{
		if (bShouldPrintDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("GAME TAKING LONGER TO LOAD PLEASE WAIT"));
		}
		//add some widget showcase
		return;
	}
	GetWorldTimerManager().ClearTimer(LoadingPeriodTimer);
	LoadingPeriodTimer.Invalidate();
	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME IS STARTING"));
	}

	FPlayerCustomiserSpawn* TeamPtr = PerTeamCustomiserSpawnLocations.Find(ETeam::Survivor);
	if (TeamPtr == nullptr)
	{
		UE_LOG(TeamManagement, Error, TEXT("AGameplayGameMode::PrepareGame - No Spawnpoints associated with enum: ETeam::Survivor, Check the GameMode settings"));
		return;
	}

	TArray<APlayerCustomiser*> CustomiserArray;
	TeamPtr->Locations.GenerateValueArray(CustomiserArray);

	for (int i = 0; i < CustomiserArray.Num(); i++)
	{
		if (CustomiserArray[i] == nullptr) continue;
		CustomiserArray[i]->PossessGamePawn();
	}

	*/

	int PrepDays = 2;//extract preptime from MapSelector

	//OnGameStart.Broadcast(PrepDays);

}

FVector AGameplayGameMode::GetValidInGameSpawnLocation()
{
	FVector PredatorLocation;
	bool bIsValidLocation = true;

	TArray<AActor*> Players;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Players);

	TArray<FVector> PlayerLocations;
	for (AActor* Player : Players)
	{
		PlayerLocations.Add(Player->GetActorLocation());
	}

	do
	{
		PredatorLocation.X = FMath::RandRange(BottomLeftArenaCornerCoordinate.X, UpperRightArenaCornerCoordinate.X);
		PredatorLocation.Y = FMath::RandRange(BottomLeftArenaCornerCoordinate.Y, UpperRightArenaCornerCoordinate.Y);
		PredatorLocation.Z = FMath::RandRange(BottomLeftArenaCornerCoordinate.Z, UpperRightArenaCornerCoordinate.Z);

		/*
		for (FVector PlayerLocation : PlayerLocations)
		{
			if (UKismetMathLibrary::Vector_Distance(PlayerLocation, PredatorLocation) <= MinimalSpawnDistanceFromPlayer)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Invalid location (Distance = %.2f), searching for a new one..."), UKismetMathLibrary::Vector_Distance(PlayerLocation, PredatorLocation)));
				bIsValidLocation = false;
			}
		}
		*/
	} while (!bIsValidLocation);

	return PredatorLocation;
}

//void AGameplayGameMode::OnPlayerReturningToLobby(APlayerCustomiser* TriggeringPlayerCustomiser)
//{
	/*
	
	TO BE REWORKED
	if (IsValid(TriggeringPlayerCustomiser) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGameplayGameMode::OnPlayerReturningToLobby IsValid(TriggeringPlayerCustomiser) == false"));
		return;
	}

	TriggeringPlayerCustomiser->PossessMainPawn();

	//!!!!!!!!!!!!!!!!!!!!!Eventually that must be changed once there are more than two teams!!!!!!!!!!!!!!!!!!!!!!!!!!!

	TArray<FPlayerCustomiserSpawn> TeamArray;
	TArray<ETeam> EnumArray;
	PerTeamCustomiserSpawnLocations.GenerateValueArray(TeamArray);
	PerTeamCustomiserSpawnLocations.GenerateKeyArray(EnumArray);

	bool bGameShouldEnd = false;

	for (int i = 0; i < EnumArray.Num(); i++)
	{
		if (EnumArray[i] == ETeam::Teamless)
		{
			continue;
		}

		bGameShouldEnd = true;

		for (TPair<FVector, APlayerCustomiser*> CustomiserPair : TeamArray[i].Locations)
		{
			if (IsValid(CustomiserPair.Value) == false)
			{
				continue;
			}
			bGameShouldEnd = bGameShouldEnd && CustomiserPair.Value->IsPossesingMainPawn();
		}

	}

	if (!bGameShouldEnd)
	{
		return;
	}
	
	//****eventually make the other team win with some widget or some shit

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Error, TEXT("GAME ENDED : STARTING LOOTING PERIOD(%f)"), LootingPeriodLenght);
	}

	GetWorldTimerManager().SetTimer(LootingPeriodTimer, this, &AGameplayGameMode::EndGame, LootingPeriodLenght, false);
	*/
//}

void AGameplayGameMode::EndGame()
{
	/*
	TO BE REWORKED
	
	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Error, TEXT("LOOTING PERIOD ENDED : RETURNING TO LOBBY"));
	}

	TArray<FPlayerCustomiserSpawn> TeamArray;
	TArray<ETeam> EnumArray;
	PerTeamCustomiserSpawnLocations.GenerateValueArray(TeamArray);
	PerTeamCustomiserSpawnLocations.GenerateKeyArray(EnumArray);


	for (int i = 0; i < EnumArray.Num(); i++)
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
			if(CustomiserPair.Value->IsPossesingMainPawn() == false) CustomiserPair.Value->PossessMainPawn();
			CustomiserPair.Value->DestroyGamePawn();
		}

	}
	*/
	//OnGameEnd.Broadcast();
}

void AGameplayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("LOGGIN IN %s"), *NewPlayer->GetName());

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::PostLogin IsValid(World) == false"));
		return;
	}

	ASG_PlayerController* PC = Cast<ASG_PlayerController>(NewPlayer);
	if (IsValid(PC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::PostLogin IsValid(PC) == false  PC is not of class ASG_PlayerController"));
		return;
	}

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADDING %s TO PLAYER LIST"), *PC->GetName());
	}

	AllPlayers.Add(PC->GetPlayerState<APlayerState>()->GetPlayerName(), PC);
	SendPlayerListUpdate();

	PC->Client_SetInputMode(EInputMode::GameAndUI);//Must be GameOnly by default

	PC->OnConnectionCleanUp.AddDynamic(this, &AGameplayGameMode::UnRegisterRequest);


	/*This Doesn't trigger for the local player since the first login of the local player
	* is triggered before the start play. 
	We can you this to avoid registering the local player in the session twice */
	OnRegisterNeeded.Broadcast(NewPlayer);
}

void AGameplayGameMode::UnRegisterRequest(APlayerController* TriggeringPlayer)
{
	OnUnregisterNeeded.Broadcast(TriggeringPlayer);
}

void AGameplayGameMode::Logout(AController* Exiting)
{
	ASG_PlayerController* PC = Cast<ASG_PlayerController>(Exiting);
	if (IsValid(PC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::Logout IsValid(PC) == false  PC is not of class ASG_PlayerController"));
		return;
	}

	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("REMOVING %s FROM PLAYER LIST"), *PC->GetName());
	}
	
	AllPlayers.Remove(PC->GetPlayerState<APlayerState>()->GetPlayerName());
	SendPlayerListUpdate();

	Super::Logout(Exiting);
}

void AGameplayGameMode::SendPlayerListUpdate()
{
	TArray<FString> PlayerNames;
	AllPlayers.GenerateKeyArray(PlayerNames);
	OnPlayerListUpdated.Broadcast(PlayerNames);
}


void AGameplayGameMode::KickPlayer(FString PlayerName)
{
	ASG_PlayerController** PCPtr = AllPlayers.Find(PlayerName);
	if (PCPtr == nullptr)
	{
		if (bShouldPrintDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player: %s could not be kicked - No such player"), *PlayerName);
		}
		return;
	}
	
	(*PCPtr)->Client_LeaveSession(true);

}
