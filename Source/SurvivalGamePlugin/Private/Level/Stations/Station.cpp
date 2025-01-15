// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/Stations/Station.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

// Sets default values
AStation::AStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AStation::BeginPlay()
{
	Super::BeginPlay();
	
}

void AStation::StopInteracting()
{
	InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
	InteractingPlayer->bShowMouseCursor = false;
	InteractingPlayer = nullptr;
}

void AStation::InteractRequest(AController* InteractingCharacter)
{
	if (InteractingCharacter->IsLocalPlayerController() == false)
	{
		return;
	}

	InteractingPlayer = Cast<ASG_PlayerController>(InteractingCharacter);
	if (IsValid(InteractingPlayer) == false)
	{
		return;
	}
	//InteractingPlayer->OnSaveUpdated.AddDynamic(this, &AShopKeeper::UpdateWidgets);
	InteractingPlayer->Client_SetInputMode(EInputMode::UIOnly);
	InteractingPlayer->SetShowMouseCursor(true);

	//UpdateWidgets();

	//ShopWidget->AddToViewport();
}

bool AStation::bCanInteract(AController* InteractingCharacter)
{
	return true;
}

