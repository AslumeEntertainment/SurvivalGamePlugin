// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class APlayerCustomiser;
class USkeletalMeshComponent;
class USkeletalMesh;

UCLASS()
class SURVIVALGAMEPLUGIN_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

private:

	bool bIsSuperVised = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "GameMaster")
	USkeletalMeshComponent* GameMaster = nullptr;

	UPROPERTY(EditAnywhere, Category = "GameMaster")
	USkeletalMesh* UnsupervisedMesh = nullptr;

	UPROPERTY(Replicated, EditAnywhere, Category = "GameMaster")
	USkeletalMesh* SupervisedMesh = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	TMap<APlayerCustomiser*, int> Teams;

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_SetSkeletalMesh(bool InSupervised, bool IsClosing = false);

public:	

	void Open(bool InSupervisionState, FString CampaignCode);
	void Close();
};
