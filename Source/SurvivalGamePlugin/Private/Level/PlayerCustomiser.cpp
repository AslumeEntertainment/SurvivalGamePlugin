// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/PlayerCustomiser.h"

#include "Kismet/GameplayStatics.h" 
#include "Net/UnrealNetwork.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "Characters/InGameCharacter.h"



// Sets default values
APlayerCustomiser::APlayerCustomiser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


void APlayerCustomiser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(APlayerCustomiser, bIsActive, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(APlayerCustomiser, bIsTaken, COND_None, REPNOTIFY_Always);

}


void APlayerCustomiser::InteractRequest(AController* InteractingCharacter)
{
	UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest - %s"), *InteractingCharacter->GetName());
	ASG_PlayerController* PC = Cast<ASG_PlayerController>(InteractingCharacter);

	if (IsValid(PC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest IsValid(InteractingPlayer) == false"));
		return;
	}
	
	//SetOwner(PC);
	//UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest OWNR nam: %s"), *GetOwner()->GetName());
	//Server_InteractRequest(PC);

	if (PC->IsLocalPlayerController() == true)
	{
		PC->Client_SetInputMode(EInputMode::UIOnly);
	}

	if (HasAuthority() == true)
	{
		InteractingPlayer = PC;

		OnPlayerStartInteracting.Broadcast(this);
		MainPawn = PC->GetPawn();


		bIsTaken = true;
	}

	//FOR TESTING ONLY REMOVBE LATER
	Server_ToggleReady_Implementation();

}

bool APlayerCustomiser::bCanInteract(AController* InteractingCharacter)
{
	return bIsActive && !bIsTaken;
}

void APlayerCustomiser::Server_StopInteracting()
{
	OnPlayerStopInteracting.Broadcast(this);
	MainPawn = nullptr;
	GamePawn = nullptr;
	 
	if (IsValid(InteractingPlayer)  ==  true)
	{
	    InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
	    InteractingPlayer = nullptr;
	}

	bIsTaken = false;
}

void APlayerCustomiser::Server_ToggleReady_Implementation()
{
	OnPlayerReadinessChanged.Broadcast(this);
}


void APlayerCustomiser::Server_SetIsActive(bool State)
{
	bIsActive = State;
}

void APlayerCustomiser::SpawnHero(FVector Location, FRotator Rotation)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Server_SpawnHero_Implementation IsValid(World) == false"));
		return;
	}

	FActorSpawnParameters SpawnInfo;
	
	GamePawn = World->SpawnActor<AInGameCharacter>(DefaultHeroClass, Location, Rotation, SpawnInfo);
	if (IsValid(GamePawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Server_SpawnHero_Implementation IsValid(GamePawn) == false"));
		return;
	}

	GamePawn->OnCharacterDeath.AddDynamic(this, &APlayerCustomiser::InGameCharacterDead);
}

void APlayerCustomiser::InGameCharacterDead()
{
	OnInGameCharacterDeath.Broadcast(this);
}

void APlayerCustomiser::DespawnHero()
{
	if (IsValid(GamePawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Server_DespawnHero IsValid(GamePawn) == false"));
		return;
	}
	GamePawn->Destroy();
	GamePawn = nullptr;
}

void APlayerCustomiser::PossessMainPawn()
{
	//InteractingPlayer->UnPossess();
	InteractingPlayer->Possess(MainPawn);
	InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
	DespawnHero();
}

void APlayerCustomiser::PossessGamePawn()
{
	//InteractingPlayer->UnPossess();
	InteractingPlayer->Possess(GamePawn);
	InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
	
}


