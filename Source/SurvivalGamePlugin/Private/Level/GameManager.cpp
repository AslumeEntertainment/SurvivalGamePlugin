// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/GameManager.h"

#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h" 

#include "Level/TimeEventManager.h"
#include "Level/PlayerCustomiser.h"

#include "Net/UnrealNetwork.h"

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

	if (HasAuthority() == false)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameManager::BeginPlay IsValid(World) == false"));
		return;
	}

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(World, TimeEventManagerClassToSearch, FoundActors);

	if (FoundActors.IsEmpty() == true)
	{
		UE_LOG(LogTemp, Error, TEXT("AGameManager::BeginPlay FoundActors.IsEmpty() == true - Please ensure there is an actor derived of class ATimeEventManager placed in the world"));
		return;
	}

	ATimeEventManager* TimeEventManager = Cast<ATimeEventManager>(FoundActors[0]);
	OnGameStart.AddDynamic(TimeEventManager, &ATimeEventManager::StartSkyRotation);
	OnGameEnd.AddDynamic(TimeEventManager, &ATimeEventManager::StopSkyRotation);

	TArray<AActor*> ChildActors;
	APlayerCustomiser* PlayerCustomiser = nullptr;
	GetAllChildActors(ChildActors);

	for (AActor* ChildActor : ChildActors)
	{
		PlayerCustomiser = Cast<APlayerCustomiser>(ChildActor);
		if (IsValid(PlayerCustomiser) == false)
		{
			continue;
		}
		
		PlayerCustomiser->OnPlayerStartInteracting.AddDynamic(this, &AGameManager::AddPlayer);

		PlayerCustomisers.Add(PlayerCustomiser, false);
	}
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

void AGameManager::AddPlayer(APlayerCustomiser* Player)
{
	Player->UpdatePlayerReadiness(false);

	Player->OnPlayerStopInteracting.AddDynamic(this, &AGameManager::RemovePlayer);
	Player->OnPlayerReadinessChanged.AddDynamic(this, &AGameManager::ChangePlayerReadiness);
	Player->OnInGameCharacterDeath.AddDynamic(this, &AGameManager::CheckGameFinishingRules);

	OnGameClosing.AddDynamic(Player, &APlayerCustomiser::Client_StopInteracting);
	OnGameStart.AddDynamic(Player, &APlayerCustomiser::PossessGamePawn);
	OnGameEnd.AddDynamic(Player, &APlayerCustomiser::PossessMainPawn);

}

void AGameManager::RemovePlayer(APlayerCustomiser* Player)
{
	PlayerCustomisers.Emplace(Player, false);
	Player->DespawnHero();

	Player->OnPlayerStopInteracting.RemoveAll(this);
	Player->OnPlayerReadinessChanged.RemoveAll(this);
	Player->OnInGameCharacterDeath.RemoveAll(this);

	OnGameClosing.RemoveAll(Player);
	OnGameStart.RemoveAll(Player);
	OnGameEnd.RemoveAll(Player);
}

void AGameManager::ChangePlayerReadiness(APlayerCustomiser* Player)
{
	bool* State = PlayerCustomisers.Find(Player);
	if (State == nullptr)
	{
		return;
	}
	bool NewState = !*State;
	PlayerCustomisers.Emplace(Player, NewState);

	NewState ? Player->SpawnHero(DefaultSpawnLocation, DefaultSpawnRotation) : Player->DespawnHero();

	Player->UpdatePlayerReadiness(NewState);

	CheckAllPlayerReadiness();
}

void AGameManager::PrepareGame()
{
	//some terrain generation and animal/Enemy spawning
	bIsGameReady = true;
}

void AGameManager::CheckAllPlayerReadiness()
{
	bool ArePlayersReady = true;

	TArray<bool > AllPlayerCustomiserReadiness;
	PlayerCustomisers.GenerateValueArray(AllPlayerCustomiserReadiness);

	for (uint8 i = 0; i < NumPlayers; i++)
	{
		ArePlayersReady &= AllPlayerCustomiserReadiness[i];
	}

	UE_LOG(LogTemp, Warning, TEXT("PlayerReadiness: %d  for %d out of %d players"), ArePlayersReady, PlayerCustomisers.Num(), NumPlayers);

	ArePlayersReady ? TryStartGame() : TryCancelGame();
}

void AGameManager::TryStartGame()
{
	GetWorldTimerManager().SetTimer(LoadingPeriodTimer, this, &AGameManager::StartGame, 1, true, LoadingPeriodLength);
}

void AGameManager::TryCancelGame()
{
	GetWorldTimerManager().ClearTimer(LoadingPeriodTimer);
	LoadingPeriodTimer.Invalidate();
}

void AGameManager::StartGame()
{
	if (bIsGameReady == false)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(LoadingPeriodTimer);
	LoadingPeriodTimer.Invalidate();

	OnGameStart.Broadcast();
}

void AGameManager::CheckGameFinishingRules(APlayerCustomiser* Player)
{
	//some logic here
	GetWorldTimerManager().SetTimer(LoadingPeriodTimer, this, &AGameManager::FinishGame, 3);
}

void AGameManager::FinishGame()
{
	TArray<APlayerCustomiser*> PlayersToUpdate;

	for (TPair<APlayerCustomiser*, bool> Player : PlayerCustomisers)
	{
		PlayersToUpdate.Add(Player.Key);
		Player.Key->UpdatePlayerReadiness(false);
	}

	for (APlayerCustomiser* PlayerToUpdate : PlayersToUpdate)
	{
		PlayerCustomisers.Emplace(PlayerToUpdate, false);
	}

	OnGameEnd.Broadcast();
}

void AGameManager::Open(bool InSupervisionState, uint8 InNumPlayers)
{
	NumPlayers = InNumPlayers;
	SetActiveSkeletalMesh(InSupervisionState ? ESupervisionState::Supervised : ESupervisionState::Unsupervised);

	UpdateActivePlayerCustomisers(true);

}

void AGameManager::Update(FString GameCode)
{
	UE_LOG(LogTemp, Warning, TEXT("Receiving -   %s"), *GameCode);
	int DelimiterIndex = 0;

	PrepareGame();
}

void AGameManager::Close()
{
	SetActiveSkeletalMesh(ESupervisionState::Closed);

	OnGameClosing.Broadcast();

	UpdateActivePlayerCustomisers(false);
}

void AGameManager::UpdateActivePlayerCustomisers(bool State)
{
	TArray<APlayerCustomiser*> AllPlayerCustomisers;
	PlayerCustomisers.GenerateKeyArray(AllPlayerCustomisers);

	for (uint8 i = 0; i < NumPlayers; i++)
	{
		AllPlayerCustomisers[i]->SetIsActive_Server(State);
	}
}



