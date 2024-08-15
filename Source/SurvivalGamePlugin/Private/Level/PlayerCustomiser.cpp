// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/PlayerCustomiser.h"

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
	OnPlayerReadinessChanged.ExecuteIfBound();
}

void APlayerCustomiser::InteractRequest()
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
	SetOwner(OwningPlayer);
}

bool APlayerCustomiser::IsOwningPlayer(AController* CharacterToCheck)
{
	return CharacterToCheck == OwningPlayer;
}

bool APlayerCustomiser::GetIsPlayerReady()
{
	return bIsPlayerReady;
}

