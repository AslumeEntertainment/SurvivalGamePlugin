// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/Stations/Station.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

// Sets default values
AStation::AStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AStation::StopInteracting()
{
	InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
	InteractingPlayer->bShowMouseCursor = false;
	InteractingPlayer = nullptr;
}

bool AStation::InteractRequest(AController* InteractingCharacter)
{
	InteractingPlayer = Cast<ASG_PlayerController>(InteractingCharacter);
	if (IsValid(InteractingPlayer) == false)
	{
		return false;
	}

	if (InteractingCharacter->IsLocalPlayerController() == false)
	{
		return false;
	}

	InteractingPlayer->Client_SetInputMode(EInputMode::UIOnly);
	InteractingPlayer->SetShowMouseCursor(true);

	return true;
}

bool AStation::bCanInteract(AController* InteractingCharacter)
{
	return true;
}

