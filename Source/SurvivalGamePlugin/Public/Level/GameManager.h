// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headers/GeneralDelegates.h"
#include "GameManager.generated.h"

class APlayerCustomiser;
class USkeletalMeshComponent;
class USkeletalMesh;
class ATimeEventManager;

UENUM(BlueprintType)
enum class ESupervisionState : uint8
{
	Closed,
	Supervised,
	Unsupervised
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerCustomiserEnableSignature, bool, State, uint8, ID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerReadinessUISignature, uint8, Current, uint8, All);


UCLASS()
class SURVIVALGAMEPLUGIN_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

private:

	bool bIsSuperVised = false;
	bool bIsGameReady = false;

	uint8 NumPlayers = 0;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ActiveMesh)
	USkeletalMesh* ActiveMesh = nullptr;

	UFUNCTION()
	void OnRep_ActiveMesh();

	FTimerHandle LoadingPeriodTimer;
	FTimerHandle PreparingPeriodTimer;

	void UpdateActivePlayerCustomisers(bool State);

	TArray<AActor*> CurrentSpawnersToFire;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "GameMaster")
	USkeletalMeshComponent* GameMaster = nullptr;

	UPROPERTY(EditAnywhere, Category = "GameMaster")
	TMap<ESupervisionState, USkeletalMesh*> MeshesPerSupervisionState;

	UPROPERTY(EditAnywhere, Category = "GameMaster")
	TSubclassOf<APlayerCustomiser> PlayerCustomiserClass;

	UPROPERTY(VisibleInstanceOnly)
	TMap<APlayerCustomiser*, bool> PlayerCustomisers;

	UPROPERTY(EditAnywhere, Category = "GameProgression | Spawners")
	float SpawnerClusterPeriod = 0.15;

	UPROPERTY(EditAnywhere, Category = "GameProgression | Spawners")
	uint8 SpawnerClusterSize = 2;

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	FVector DefaultSpawnLocation = FVector(0,0,400);

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	FRotator DefaultSpawnRotation = FRotator(0, 0, 0);

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	float LoadingPeriodLength = 2.0f;

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	float LootingPeriodLength = 2.0f;

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	TSubclassOf<ATimeEventManager> TimeEventManagerClassToSearch;

	UPROPERTY(EditAnywhere, Category = "GameProgression | Spawners")
	class ALevelCleaner* LevelCleaner;

	UPROPERTY(EditAnywhere, Category = "GameProgression | Spawners")
	FName EnemySpawnerTag;

	UPROPERTY(EditAnywhere, Category = "GameProgression | Spawners")
	TMap<class AActorSpawner*, int> EnemySpawnerMap;

	TArray<FTimerHandle* > ActiveSpawnTimers;

	int RemainingPlayers = 0;
	int RemainingEnemies = 0;

	UFUNCTION()
	void SpawnEnemy(AActorSpawner* Spawner);
	UFUNCTION()
	void EnemyDied();
	void SetActiveSkeletalMesh(ESupervisionState State);
	void UpdateSkeletalMesh();

	UFUNCTION()
	void AddPlayer(APlayerCustomiser* Player);

	UFUNCTION()
	void RemovePlayer(APlayerCustomiser* Player);

	UFUNCTION()
	void ChangePlayerReadiness(APlayerCustomiser* Player);

	UFUNCTION()
	void PrepareGame();

	UFUNCTION()
	void SpawnCluster();

	void CheckAllPlayerReadiness();

	UFUNCTION()
	void TryStartGame();

	UFUNCTION()
	void TryCancelGame();

	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void CheckGameFinishingRules(APlayerCustomiser* Player);

	void FinishGame();

	FPlayerCustomiserEnableSignature OnPlayerCustomiserEnable;
	FPlayerReadinessUISignature OnReadinessUI;

	FBooleanTransferSignature OnStateChanged;
	FVoidDataTransferSignature OnGameClosing;

	FVoidDataTransferSignature OnGameStart;
	FVoidDataTransferSignature OnGameEnd;

public:	



	void Open(bool InSupervisionState, uint8 InNumPlayers);
	void Update(FString GameCode);
	void Close();

	FVoidDataTransferSignature OnGameProgressed;
	

};
