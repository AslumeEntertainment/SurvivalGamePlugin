// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LobbyManager.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"
#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/GameInstance/SG_GameInstance.h"
#include "Level/GameManager.h"
#include "Headers/GameManagingStructs.h"

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

void ALobbyManager::SetCampaignName(FString Name)
{
	CampaignName = Name;
	OnCampaignNameChanged.Broadcast(CampaignName);
}

void ALobbyManager::SetNumPlayers(uint8 InNumPlayers)
{
	NumPlayers = InNumPlayers;
	OnPlayerNumChanged.Broadcast(NumPlayers);
}

void ALobbyManager::SetIsSupervised(bool InIsSupervised)
{
	bIsSupervised = InIsSupervised;
	OnIsSupervisedChanged.Broadcast(bIsSupervised);
}


void ALobbyManager::OpenPlaySpace()
{
	if (HasAuthority() == false)
	{
		return;
	}

	USG_GameInstance* GameInstance = Cast<USG_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ALobbyManager::CreateLobby IsValid(GameInstance) == false - GameInstance may not be of class USG_GameInstance"));
		return;
	}

	uint8 NumConnections;
	switch (NumPlayers)
	{
		case 1: case 2: NumConnections = 2; break;
		case 3: case 4: NumConnections = 4; break;
		case 5: case 6: NumConnections = 6; break;
		case 7: case 8: NumConnections = 8; break;
		default: NumConnections = 0;
	}

	AGameManager** GameManagerPtr = TablesByPlayerNum.Find(NumConnections);
	if (GameManagerPtr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Table for %d players"), NumConnections);
		return;
	}
	ActiveGameManager = *GameManagerPtr;
	ActiveGameManager->Open(bIsSupervised, CurrentGames[CurrentGame]);

}

void ALobbyManager::ClosePlaySpace()
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (IsValid(ActiveGameManager) == false)
	{
		return;
	}
	ActiveGameManager->Close();
	ActiveGameManager = nullptr;
}

void ALobbyManager::LeaveLobby()
{
	ASG_PlayerController* PC = Cast<ASG_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	PC->Client_LeaveSession(true);
	if (IsValid(ActiveGameManager) == false)
	{
		return;
	}
	ActiveGameManager->Close();
	ActiveGameManager = nullptr;
}

void ALobbyManager::ReceiveCampaignID(FName ID)
{
	if (HasAuthority() == false)
	{
		return;
	}

	FCampaignPersistentData* CampaignDataPtr = CampaignDataTable->FindRow<FCampaignPersistentData>(ID, "CampaignSearch");
	if (CampaignDataPtr == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ALobbyManager::ReceiveCampaignID CampaignData == nullptr - No row in data table(%s) with name %s"), *CampaignDataTable->GetName(), *ID.ToString());
		return;
	}

	ReceiveCampaignCode(CampaignDataPtr->Code);
}

void ALobbyManager::ReceiveCampaignCode(FString Code)
{
	if (HasAuthority() == false)
	{
		return;
	}

	CurrentGames.Empty();

	FString ExtractedCampaignName = "";
	FString TempString = "";
	uint8 ExtractedNumPlayers = 0;
	bool ExtractedIsSupervised = false;

	FString StrippedCode = "";
	int DelimiterIndex = 0;

	//Getting the name of the Campaign
	Code.FindChar(':', DelimiterIndex);
	ExtractedCampaignName = Code.Left(DelimiterIndex);
	Code = Code.Mid(DelimiterIndex + 1);

	//Getting the number of players
	Code.FindChar(':', DelimiterIndex);
	ExtractedNumPlayers = UKismetStringLibrary::Conv_StringToInt(Code.Left(DelimiterIndex));
	Code = Code.Mid(DelimiterIndex + 1);

	//getting the supervision status
	Code.FindChar(':', DelimiterIndex);
	TempString = Code.Left(DelimiterIndex);
	if (TempString == "c")
	{
		OnSupervisionChangeable.Broadcast();
	}
	ExtractedIsSupervised = TempString.ToBool();
	Code = Code.Mid(DelimiterIndex + 1);

	SetCampaignName(ExtractedCampaignName);
	SetNumPlayers(ExtractedNumPlayers);
	SetIsSupervised(ExtractedIsSupervised);

	//CampaignCode = Code;

	uint8 ExtractedNumberOfGames = 0;

	//Getting the number of games
	Code.FindChar('[', DelimiterIndex);
	ExtractedNumberOfGames = UKismetStringLibrary::Conv_StringToInt(Code.Left(DelimiterIndex));
	Code = Code.Mid(DelimiterIndex + 1);

	UE_LOG(LogTemp, Error, TEXT("Code: %s"), *Code);

	FString GameString;

	for (uint8 i = 0; i < ExtractedNumberOfGames; i++)
	{
		Code.FindChar('-', DelimiterIndex);
		GameString = Code.Left(DelimiterIndex);
		Code = Code.Mid(DelimiterIndex + 1);

		UE_LOG(LogTemp, Error, TEXT("Game %d: %s"), i, *GameString);
		CurrentGames.Add(GameString);
	}

}


