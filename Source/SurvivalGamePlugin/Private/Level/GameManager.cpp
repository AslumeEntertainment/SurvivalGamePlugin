// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/GameManager.h"

#include "Kismet/KismetStringLibrary.h"

#include "Level/PlayerCustomiser.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GameMaster = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GameMaster"));
	GameMaster->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGameManager::NetMulticast_SetSkeletalMesh_Implementation(bool InSupervised, bool IsClosing)
{
	if (IsClosing)
	{
		GameMaster->SetSkeletalMesh(nullptr);
		return;
	}
	InSupervised ? GameMaster->SetSkeletalMesh(SupervisedMesh) : GameMaster->SetSkeletalMesh(UnsupervisedMesh);
}

void AGameManager::Open(bool InSupervisionState, FString CampaignCode)
{
	NetMulticast_SetSkeletalMesh(InSupervisionState);

	uint8 ExtractedNumberOfGames = 0;

	int DelimiterIndex = 0;

	//Getting the number of games
	CampaignCode.FindChar(':', DelimiterIndex);
	ExtractedNumberOfGames = UKismetStringLibrary::Conv_StringToInt(CampaignCode.Left(DelimiterIndex));
	CampaignCode = CampaignCode.Mid(DelimiterIndex + 1);

	UE_LOG(LogTemp, Error, TEXT("Number of games: %d"), ExtractedNumberOfGames);

}

void AGameManager::Close()
{
	NetMulticast_SetSkeletalMesh(false, true);
}


