// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/PlayerCustomiser.h"
#include "Characters/InGameCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayerCustomiser::APlayerCustomiser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

/* Called when the game starts or when spawned
void APlayerCustomiser::BeginPlay()
{
	Super::BeginPlay();
	
}*/

/* Called every frame
void APlayerCustomiser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

void APlayerCustomiser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(APlayerCustomiser, bIsPlayerReady, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(APlayerCustomiser, OwningPlayer, COND_None, REPNOTIFY_Always);
}

void APlayerCustomiser::ServerToggleReady_Implementation()
{
	bIsPlayerReady = !bIsPlayerReady;
	OnPlayerReadinessChanged.Broadcast();
}

void APlayerCustomiser::InteractRequest(AController* InteractingCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Interacting with Customiser"));
	ServerToggleReady();
}

bool APlayerCustomiser::bCanInteract(AController* InteractingCharacter)
{
	return IsOwningPlayer(InteractingCharacter);
}

void APlayerCustomiser::SetOwningPlayer(AController* InOwner)
{
	OwningPlayer = InOwner;
	MainPawn = OwningPlayer->GetPawn();
	SetOwner(OwningPlayer);
}

void APlayerCustomiser::SetGamePawn(APawn* InGamePawn)
{

	AInGameCharacter* Character = Cast<AInGameCharacter>(InGamePawn);

	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::SetGamePawn IsValid(Character) == false  Character Is Not Of Class: AInGameCharacter"));
		return;
	}

	Character->OnCharacterDeath.AddDynamic(this, &APlayerCustomiser::AnnounceDeath);

	GamePawn = InGamePawn;
}

void APlayerCustomiser::AnnounceDeath()
{
	OnGamePawnDeath.Broadcast(this);
}

bool APlayerCustomiser::IsOwningPlayer(AController* CharacterToCheck)
{
	return CharacterToCheck == OwningPlayer;
}

bool APlayerCustomiser::GetIsPlayerReady()
{
	return bIsPlayerReady;
}

bool APlayerCustomiser::IsPossesingMainPawn()
{
	return MainPawn == OwningPlayer->GetPawn();
}

void APlayerCustomiser::PossessMainPawn()
{
	if (IsValid(MainPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::PossessMainPawn IsValid(MainPawn) == false Not right call order"));
		return;
	}
	MainPawn->SetActorTickEnabled(true);
	OwningPlayer->Possess(MainPawn);

}

void APlayerCustomiser::PossessGamePawn()
{
	if (IsValid(GamePawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::PossessGamePawn IsValid(GamePawn) == false Not right call order"));
		return;
	}
	OwningPlayer->Possess(GamePawn);
	MainPawn->SetActorTickEnabled(false);
}


void APlayerCustomiser::DestroyGamePawn()
{
	GamePawn->Destroy();
}