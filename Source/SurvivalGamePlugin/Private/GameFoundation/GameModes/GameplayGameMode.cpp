// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameModes/GameplayGameMode.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"

//#include "GameFoundation/GameInstance/SessionSubsystem.h"
//#include "Level/PlayerCustomiser.h"
#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(TeamManagement, Log, All);

void AGameplayGameMode::StartPlay()
{
	UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay"));

	Super::StartPlay();

	//USessionSubsystem* SessionSubsystemPtr = GetGameInstance()->GetSubsystem<UAGN_SessionSubsystem>();
	//if (IsValid(SessionSubsystemPtr) == false)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay (IsValid(SessionSubsystem) == false"));
	//	return;
	//}

	//SessionSubsystemPtr->OnHostedGameClosing.AddDynamic(this, &AGameplayGameMode::KickAllPlayers);
	//OnRegisterNeeded.AddDynamic(SessionSubsystemPtr, &USessionSubsystem::RegisterNewPlayer);
	//OnUnregisterNeeded.AddDynamic(SessionSubsystemPtr, &USessionSubsystem::UnRegisterPlayer);
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
