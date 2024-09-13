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

void AGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AGameManager, ActiveMesh, COND_None, REPNOTIFY_Always);
}

void AGameManager::OnRep_ActiveMesh()
{
	UpdateSkeletalMesh();
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGameManager::SetActiveSkeletalMesh(ESupervisionState State)
{
	USkeletalMesh** MeshPtr = MeshesPerSupervisionState.Find(State);
	if (MeshPtr == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameManager::SetActiveSkeletalMesh No mesh specified for state: %s"), *UEnum::GetValueAsString(State));
		return;
	}
	ActiveMesh = *MeshPtr;

	UpdateSkeletalMesh();
}

void AGameManager::UpdateSkeletalMesh()
{
	GameMaster->SetSkeletalMesh(ActiveMesh);
}

void AGameManager::Open(bool InSupervisionState, FString CampaignCode)
{
	SetActiveSkeletalMesh(InSupervisionState ? ESupervisionState::Supervised : ESupervisionState::Unsupervised);

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
	SetActiveSkeletalMesh(ESupervisionState::Closed);
}


