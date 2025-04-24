// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/GameManager.h"

#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h" 

#include "Level/ActorSpawner.h"
#include "Level/LevelCleaner.h"
#include "Level/TimeEventManager.h"
#include "Level/Stations/PlayerCustomiser.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AGameManager::AGameManager()
{
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

	

	TArray<UActorComponent*> ChildActors = GetComponentsByTag(USceneComponent::StaticClass(), FName("ChairSpawnPoint"));
	APlayerCustomiser* PlayerCustomiser = nullptr;

	uint8 PCID = 0;
	for (UActorComponent* ChildActor : ChildActors)
	{
		///MAY BE BETTER TO DESTROY COMPONENT AFTER SPAWN BUT FOR NOW THAT WILL SUFICE
		USceneComponent* SpawnPoint = Cast<USceneComponent>(ChildActor);
		if (IsValid(SpawnPoint) == false) continue;

		PlayerCustomiser = World->SpawnActor<APlayerCustomiser>(PlayerCustomiserClass, SpawnPoint->GetComponentTransform());
		if (IsValid(PlayerCustomiser) == false)
		{
			continue;
		}
		
		PlayerCustomiser->Init(PCID++);
		OnPlayerCustomiserEnable.AddDynamic(PlayerCustomiser, &APlayerCustomiser::SetIsActive_Server);
		PlayerCustomiser->OnPlayerStartInteracting.AddDynamic(this, &AGameManager::AddPlayer);

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
	PlayerCustomisers.Add(Player, false);

	Player->UpdatePlayerReadiness(false);

	
	Player->OnPlayerStopInteracting.AddDynamic(this, &AGameManager::RemovePlayer);
	Player->OnPlayerReadinessChanged.AddDynamic(this, &AGameManager::ChangePlayerReadiness);
	Player->OnInGameCharacterDeath.AddDynamic(this, &AGameManager::CheckGameFinishingRules);

	OnReadinessUI.AddDynamic(Player, &APlayerCustomiser::Client_UpdateReadinessUI);
	OnGameClosing.AddDynamic(Player, &APlayerCustomiser::Client_StopInteracting);
	OnGameStart.AddDynamic(Player, &APlayerCustomiser::PossessGamePawn);
	OnGameEnd.AddDynamic(Player, &APlayerCustomiser::PossessMainPawn);

	CheckAllPlayerReadiness();

}

void AGameManager::RemovePlayer(APlayerCustomiser* Player)
{
	PlayerCustomisers.Remove(Player);
	Player->DespawnHero();

	Player->OnPlayerStopInteracting.RemoveAll(this);
	Player->OnPlayerReadinessChanged.RemoveAll(this);
	Player->OnInGameCharacterDeath.RemoveAll(this);

	OnReadinessUI.RemoveAll(Player);
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
	CurrentSpawnersToFire.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorSpawner::StaticClass(), CurrentSpawnersToFire);
	
	GetWorldTimerManager().SetTimer(PreparingPeriodTimer, this, &AGameManager::SpawnCluster, SpawnerClusterPeriod, true, 0);
	//some terrain generation and animal/Enemy spawning
	
}

void AGameManager::SpawnCluster()
{
	for (int i = 0; i < SpawnerClusterSize; i++)
	{
		if (CurrentSpawnersToFire.IsValidIndex(SpawnerClusterSize) == false) { bIsGameReady = true; return; }

		AActorSpawner* ActorSpawner = Cast<AActorSpawner>(CurrentSpawnersToFire[SpawnerClusterSize]);
		if (IsValid(ActorSpawner) == false) continue;
		ActorSpawner->Spawn();
		CurrentSpawnersToFire.RemoveAt(SpawnerClusterSize);
		
	}
	
}

void AGameManager::CheckAllPlayerReadiness()
{

	TArray<bool > AllPlayerCustomiserReadiness;
	PlayerCustomisers.GenerateValueArray(AllPlayerCustomiserReadiness);

	uint8 NumReadyPlayers = 0;
	for (uint8 i = 0; i < AllPlayerCustomiserReadiness.Num(); i++)
	{
		if (AllPlayerCustomiserReadiness[i] == true) NumReadyPlayers++;
	}

	OnReadinessUI.Broadcast(NumReadyPlayers, NumPlayers);

	UE_LOG(LogTemp, Warning, TEXT("PlayerReadiness: %d  for %d out of %d players"), NumReadyPlayers == NumPlayers, PlayerCustomisers.Num(), NumPlayers);

	(NumReadyPlayers == NumPlayers) ? TryStartGame() : TryCancelGame();
}

void AGameManager::TryStartGame()
{
	PrepareGame();
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
	GetWorldTimerManager().SetTimer(LoadingPeriodTimer, this, &AGameManager::FinishGame, 10);
}

void AGameManager::FinishGame()
{
	TArray<APlayerCustomiser*> PlayersToUpdate;

	for (TPair<APlayerCustomiser*, bool> Player : PlayerCustomisers)
	{
		PlayerCustomisers.Emplace(Player.Key, false);
		Player.Key->UpdatePlayerReadiness(false);
	}

	OnReadinessUI.Broadcast(0, NumPlayers);

	OnGameEnd.Broadcast();

	LevelCleaner->Clean();
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

	
}

void AGameManager::Close()
{
	SetActiveSkeletalMesh(ESupervisionState::Closed);

	OnGameClosing.Broadcast();

	UpdateActivePlayerCustomisers(false);
}

void AGameManager::UpdateActivePlayerCustomisers(bool State)
{
	for (uint8 i = 0; i < NumPlayers; i++)
	{
		OnPlayerCustomiserEnable.Broadcast(State, i);
	}
}



