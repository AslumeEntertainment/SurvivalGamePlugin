// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headers/GeneralDelegates.h"
#include "GameManager.generated.h"

class APlayerCustomiser;
class USkeletalMeshComponent;
class USkeletalMesh;

UENUM(BlueprintType)
enum class ESupervisionState : uint8
{
	Closed,
	Supervised,
	Unsupervised
};

UCLASS()
class SURVIVALGAMEPLUGIN_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

private:

	bool bIsSuperVised = false;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ActiveMesh)
	USkeletalMesh* ActiveMesh = nullptr;

	UFUNCTION()
	void OnRep_ActiveMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "GameMaster")
	USkeletalMeshComponent* GameMaster = nullptr;

	UPROPERTY(EditAnywhere, Category = "GameMaster")
	TMap<ESupervisionState, USkeletalMesh*> MeshesPerSupervisionState;

	UPROPERTY(VisibleDefaultsOnly)
	TMap<APlayerCustomiser*, int> Teams;

	void SetActiveSkeletalMesh(ESupervisionState State);
	void UpdateSkeletalMesh();

public:	

	void Open(bool InSupervisionState, FString CampaignCode);
	void Close();

	FVoidDataTransferSignature OnGameProgressed;

};
