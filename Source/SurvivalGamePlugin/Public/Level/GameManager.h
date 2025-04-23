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

	void UpdateActivePlayerCustomisers(bool State);

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

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	FVector DefaultSpawnLocation = FVector(0,0,400);

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	FRotator DefaultSpawnRotation = FRotator(0, 0, 0);

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	float LoadingPeriodLength = 2.0f;

	UPROPERTY(EditAnywhere, Category = "GameProgression")
	TSubclassOf<ATimeEventManager> TimeEventManagerClassToSearch;

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
