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

		OnStateChanged.AddDynamic(PlayerCustomiser, &APlayerCustomiser::Server_SetIsActive);
		OnGameClosing.AddDynamic(PlayerCustomiser, &APlayerCustomiser::Server_StopInteracting);
		OnGameStart.AddDynamic(PlayerCustomiser, &APlayerCustomiser::Server_PossessGamePawn);
		OnGameEnd.AddDynamic(PlayerCustomiser, &APlayerCustomiser::Server_PossessMainPawn);
		
		PlayerCustomiser->OnPlayerStartInteracting.AddDynamic(this, &AGameManager::AddPlayer);
		PlayerCustomiser->OnPlayerStopInteracting.AddDynamic(this, &AGameManager::RemovePlayer);
		PlayerCustomiser->OnPlayerReadinessChanged.AddDynamic(this, &AGameManager::ChangePlayerReadiness);
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
	Players.Add(Player, false);
}

void AGameManager::RemovePlayer(APlayerCustomiser* Player)
{
	Players.Remove(Player);
}

void AGameManager::ChangePlayerReadiness(APlayerCustomiser* Player)
{
	bool* State = Players.Find(Player);
	if (State == nullptr)
	{
		return;
	}
	bool NewState = !*State;
	Players.Emplace(Player, NewState);

	if (NewState == false)
	{
		Player->Server_DespawnHero();
		return;
	}

	Player->Server_SpawnHero(DefaultSpawnLocation, DefaultSpawnRotation);

	CheckAllPlayerReadiness();
}

void AGameManager::PrepareGame()
{
	//some terrain generation
	bIsGameReady = true;
}

void AGameManager::CheckAllPlayerReadiness()
{
	bool ArePlayersReady = true;
	for (TPair<APlayerCustomiser*, bool> Player : Players)
	{
		ArePlayersReady |= Player.Value;
		
	}

	UE_LOG(LogTemp, Warning, TEXT("PlayerReadiness: %d  for %d out of %d players"), ArePlayersReady, Players.Num(), NumPlayers);

	(ArePlayersReady && (Players.Num() == NumPlayers)) ? TryStartGame() : TryCancelGame();
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
	FinishGame();
}

void AGameManager::FinishGame()
{
	OnGameEnd.Broadcast();
}

void AGameManager::Open(bool InSupervisionState, uint8 InNumPlayers)
{
	NumPlayers = InNumPlayers;
	SetActiveSkeletalMesh(InSupervisionState ? ESupervisionState::Supervised : ESupervisionState::Unsupervised);

	OnStateChanged.Broadcast(true);
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
	OnStateChanged.Broadcast(false);
}




