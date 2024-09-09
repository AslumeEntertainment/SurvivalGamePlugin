// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

#include "Kismet/GameplayStatics.h" 

#include "GameFoundation/GameInstance/SessionSubsystem.h"


void ASG_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (bIsCamersMovementConstrained)
	{
		PlayerCameraManager->ViewYawMin = -80.f;
		PlayerCameraManager->ViewYawMax = 80.f;
		PlayerCameraManager->ViewPitchMin = -55.f;
		PlayerCameraManager->ViewPitchMax = 55.f;
	}

}

void ASG_PlayerController::Client_SetInputMode_Implementation(EInputMode InputMode)
{
	switch (InputMode)
	{
		case EInputMode::GameOnly: SetInputMode(FInputModeGameOnly()); break;
		case EInputMode::GameAndUI : SetInputMode(FInputModeGameAndUI()); break;
		case EInputMode::UIOnly: SetInputMode(FInputModeUIOnly()); break;
	}
}

void ASG_PlayerController::Client_LeaveSession_Implementation(bool bWasKicked)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::Client_LeaveSession_Implementation IsValid(World) == false"));
		return;
	}

	USessionSubsystem* SessionSubsystemPtr = GetGameInstance()->GetSubsystem<USessionSubsystem>();
	if (IsValid(SessionSubsystemPtr) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameplayGameMode::StartPlay (IsValid(SessionSubsystem) == false"));
		return;
	}

	SessionSubsystemPtr->DestroySession();

}

void ASG_PlayerController::OnNetCleanup(UNetConnection* Connection)
{
	if (GetLocalRole() == ROLE_Authority && PlayerState != NULL)
	{

		OnConnectionCleanUp.ExecuteIfBound(this);
	}

	Super::OnNetCleanup(Connection);
}
