// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

#include "Kismet/GameplayStatics.h" 

#include "GameFoundation/GameInstance/SG_GameInstance.h"

void ASG_PlayerController::Client_SetInputMode_Implementation(EInputMode InputMode)
{
	switch (InputMode)
	{
		case EInputMode::GameOnly: SetInputMode(FInputModeGameOnly()); break;
		case EInputMode::GameAndUI : SetInputMode(FInputModeGameAndUI()); break;
		case EInputMode::UIOnly: SetInputMode(FInputModeUIOnly()); break;
	}
}

void ASG_PlayerController::Client_SetLocalSessionState_Implementation(ESessionState InSessionState)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::Client_SetLocalSessionState_Implementation IsValid(World) == false"));
		return;
	}

	USG_GameInstance* GameInstance = Cast<USG_GameInstance>(UGameplayStatics::GetGameInstance(World));
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::Client_SetLocalSessionState_Implementation IsValid(GameInstance) == false"));
		return;
	}

	GameInstance->LocalSessionState = InSessionState;
}

void ASG_PlayerController::Client_LeaveSession_Implementation(bool bWasKicked)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::Client_LeaveSession_Implementation IsValid(World) == false"));
		return;
	}

	USG_GameInstance* GameInstance = Cast<USG_GameInstance>(UGameplayStatics::GetGameInstance(World));
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::Client_LeaveSession_Implementation IsValid(GameInstance) == false"));
		return;
	}
	if (GameInstance->LocalSessionState != ESessionState::Hosting)
	{
		UE_LOG(LogTemp, Display, TEXT("CLIENT: CLIENT TRAVEL TO IDLE STATE"));
		ClientTravel(GameInstance->LobbyMapName, ETravelType::TRAVEL_Absolute);
	}

	GameInstance->DestroySession();

}

void ASG_PlayerController::OnNetCleanup(UNetConnection* Connection)
{
	if (GetLocalRole() == ROLE_Authority && PlayerState != NULL)
	{
		UWorld* World = GetWorld();
		if (IsValid(World) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::OnNetCleanup IsValid(World) == false"));
			return;
		}

		USG_GameInstance* GameInstance = Cast<USG_GameInstance>(UGameplayStatics::GetGameInstance(World));
		if (IsValid(GameInstance) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::OnNetCleanup IsValid(GameInstance) == false"));
			return;
		}

		GameInstance->UnRegisterPlayer(this);
	}

	Super::OnNetCleanup(Connection);
}
