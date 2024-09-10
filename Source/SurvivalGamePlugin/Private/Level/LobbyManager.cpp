// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LobbyManager.h"

#include "Kismet/GameplayStatics.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"
#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/GameInstance/SG_GameInstance.h"
#include "Level/GameManager.h"

// Sets default values
ALobbyManager::ALobbyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALobbyManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALobbyManager::SetNumPlayers(int InNumPlayers)
{
	NumPlayers = InNumPlayers;
}

void ALobbyManager::CreateLobby()
{

	USG_GameInstance* GameInstance = Cast<USG_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ALobbyManager::CreateLobby IsValid(GameInstance) == false - GameInstance may not be of class USG_GameInstance"));
		return;
	}

	int NumConections;
	switch (NumPlayers)
	{
		case 1: case 2: NumConections = 2; break;
		case 3: case 4: NumConections = 4; break;
		case 5: case 6: NumConections = 6; break;
		case 7: case 8: NumConections = 8; break;
		default: NumConections = 1;
	}

	AGameManager** GameManagerPtr = TablesByPlayerNum.Find(NumConections);
	if (GameManagerPtr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Table for %d players"), NumConections);
		return;
	}
	ActiveGameManager = *GameManagerPtr;
	ActiveGameManager->Open();

}

void ALobbyManager::LeaveLobby()
{
	ASG_PlayerController* PC = Cast<ASG_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	PC->Client_LeaveSession(true);
	ActiveGameManager->Close();
	ActiveGameManager = nullptr;
}


