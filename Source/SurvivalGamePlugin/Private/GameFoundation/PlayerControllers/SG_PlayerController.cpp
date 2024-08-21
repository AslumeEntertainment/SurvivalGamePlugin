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
	}

	Super::OnNetCleanup(Connection);
}
