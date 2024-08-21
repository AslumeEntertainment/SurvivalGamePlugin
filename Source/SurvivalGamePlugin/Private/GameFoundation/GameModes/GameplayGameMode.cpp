// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameModes/GameplayGameMode.h"

#include "Kismet/GameplayStatics.h" 
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameFoundation/GameInstance/SG_GameInstance.h"
#include "Level/TeamManager.h"
#include "Level/PlayerCustomiser.h"
#include "Level/TimeEventManager.h"
#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

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

	FoundActors.Empty();
	
	UGameplayStatics::GetAllActorsOfClass(World, TimeEventManagerClassToSearch, FoundActors);

	if (FoundActors.IsEmpty() == true)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay FoundActors.IsEmpty() == true - Please ensure there is an actor derived of class ATimeEventManager placed in the world"));
		return;
	}

	ATimeEventManager* TimeEventManager = Cast<ATimeEventManager>(FoundActors[0]);
	TimeEventManager->OnPreparationOver.BindDynamic(this, &AGameplayGameMode::SpawnPredators);
	OnGameStart.AddDynamic(TimeEventManager, &ATimeEventManager::StartPreparationPeriod);
	OnGameEnd.AddDynamic(TimeEventManager, &ATimeEventManager::StopSkyRotation);
}

void AGameplayGameMode::JoinTeam(AController* TriggeringPlayer, ETeam TeamToJoin)
{
	if(IsThereSpaceInTeam(TeamToJoin) == false)
	{
		UE_LOG(TeamManagement, Display, TEXT("AGameplayGameMode::JoinTeam - No free space in team %s"), *UEnum::GetValueAsString(TeamToJoin));
		return;
	}

	ETeam CurrentTeam = GetCurrentTeam(TriggeringPlayer);

	DestroyPlayerCustomiser(TriggeringPlayer, CurrentTeam);
	
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
	TArray<FPlayerCustomiserSpawn> TeamArray;
	TArray<ETeam> EnumArray;
	PerTeamCustomiserSpawnLocations.GenerateValueArray(TeamArray);
	PerTeamCustomiserSpawnLocations.GenerateKeyArray(EnumArray);

	for (int i=0; i< TeamArray.Num() ; i++)
	{
		TArray<APlayerCustomiser*> CustomiserArray;
		TeamArray[i].Locations.GenerateValueArray(CustomiserArray);

		for (APlayerCustomiser* Customiser : CustomiserArray)
		{
			if (Customiser == nullptr) continue;
			if (Customiser->IsOwningPlayer(TriggeringPlayer))
			{
				PlayerTeam = EnumArray[i];
				break;
			}
		}
	}

	return PlayerTeam;
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
		PlayerCustomiser->OnPlayerReadinessChanged.AddDynamic(this, &AGameplayGameMode::OnPlayerStatusUpdated);
		PlayerCustomiser->OnGamePawnDeath.AddDynamic(this, &AGameplayGameMode::OnPlayerReturningToLobby);

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
	

	PrepareGame();

}

void AGameplayGameMode::PrepareGame()
{
	GetWorldTimerManager().SetTimer(LoadingPeriodTimer, this, &AGameplayGameMode::StartGame, 1, true, LoadingPeriodLenght);

	//Start Generating map

	SpawnSurvivors();
}

void AGameplayGameMode::StartGame()
{
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

	int PrepDays = 2;//extract preptime from MapSelector

	OnGameStart.Broadcast(PrepDays);

}

void AGameplayGameMode::SpawnSurvivors()
{
	FPlayerCustomiserSpawn* TeamPtr = PerTeamCustomiserSpawnLocations.Find(ETeam::Survivor);
	if (TeamPtr == nullptr)
	{
		UE_LOG(TeamManagement, Error, TEXT("AGameplayGameMode::SpawnSurvivors - No Spawnpoints associated with enum: ETeam::Survivor, Check the GameMode settings"));
		return;
	}

	TArray<APlayerCustomiser*> CustomiserArray;
	TeamPtr->Locations.GenerateValueArray(CustomiserArray);

	for (int i = 0; i < CustomiserArray.Num(); i++)
	{
		if (CustomiserArray[i] == nullptr) continue;

		UWorld* World = GetWorld();
		if (IsValid(World) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::SpawnSurvivors IsValid(World) == false"));
			return;
		}

		//here get the actual classs once the account logic is done

		ACharacter* GamePawn = World->SpawnActor<ACharacter>(DebugSurvivorClass, GetValidInGameSpawnLocation(), FRotator(), FActorSpawnParameters());

		if (IsValid(GamePawn) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::SpawnSurvivors IsValid(PlayerCustomiser) == false"));
			return;
		}

		CustomiserArray[i]->SetGamePawn(GamePawn);
	}
}

void AGameplayGameMode::SpawnPredators()
{
	if (bShouldPrintDebug)
	{
		UE_LOG(LogTemp, Error, TEXT("PREDATOR IS SPAWNING"));
	}

	FPlayerCustomiserSpawn* TeamPtr = PerTeamCustomiserSpawnLocations.Find(ETeam::Hunter);
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

		UWorld* World = GetWorld();
		if (IsValid(World) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::PrepareGame IsValid(World) == false"));
			return;
		}

		//here get the actual classs once the account logic is done

		ACharacter* GamePawn = World->SpawnActor<ACharacter>(DebugHunterClass, GetValidInGameSpawnLocation(), FRotator(), FActorSpawnParameters());

		if (IsValid(GamePawn) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::PrepareGame IsValid(PlayerCustomiser) == false"));
			return;
		}

		//some fade in for the players
		CustomiserArray[i]->SetGamePawn(GamePawn);
		CustomiserArray[i]->PossessGamePawn();
	}
}

FVector AGameplayGameMode::GetValidInGameSpawnLocation()
{
	FVector PredatorLocation;
	bool bIsValidLocation = true;

	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Players);

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

void AGameplayGameMode::OnPlayerReturningToLobby(APlayerCustomiser* TriggeringPlayerCustomiser)
{

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
	
}

void AGameplayGameMode::EndGame()
{
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

	OnGameEnd.Broadcast();
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

	ASG_PlayerController* PC = Cast<ASG_PlayerController>(NewPlayer);
	if (IsValid(PC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::PostLogin IsValid(PC) == false  PC is not of class ASG_PlayerController"));
		return;
	}

	PC->Client_SetInputMode(EInputMode::GameOnly);
}




