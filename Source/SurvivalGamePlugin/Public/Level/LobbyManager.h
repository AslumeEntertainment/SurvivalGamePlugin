// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Headers/GeneralDelegates.h"
#include "Headers/GameManagingPersistentStructs.h"

#include "LobbyManager.generated.h"

class AGameManager;
class UDataTable;


struct PerCampaignData
{
	FCampaignPersistentData Campaign;

	uint8 Progression = 0;
};

UCLASS()
class SURVIVALGAMEPLUGIN_API ALobbyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyManager();

private:

	TMap<FName, PerCampaignData> PerCampaignProgression;

	FString CampaignName = "";
	uint8 NumPlayers = 0;
	bool bIsSupervised = false;
	uint8 CurrentGame = 0;

	
	AGameManager* ActiveGameManager = nullptr;
	TArray<FString> CurrentGames;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldPrintDebug = true;

	UPROPERTY(EditAnywhere, Category = "GameManaging")
	TMap<int, AGameManager*> TablesByPlayerNum;

public:

	UPROPERTY(EditAnywhere)
	UDataTable* CampaignDataTable;

	UFUNCTION(BlueprintCallable)
	void SetCampaignName(FString Name);

	UFUNCTION(BlueprintCallable)
	void SetNumPlayers(uint8 InNumPlayers);

	UFUNCTION(BlueprintCallable)
	void SetIsSupervised(bool InIsSupervised);

	UFUNCTION(BlueprintCallable)
	void OpenPlaySpace();

	UFUNCTION(BlueprintCallable)
	void ClosePlaySpace();

	UFUNCTION(BlueprintCallable)
	void LeaveLobby();

	UFUNCTION(BlueprintCallable)
	void ReceiveCampaignID(FName ID);

	UFUNCTION(BlueprintCallable)
	void ReceiveCampaignCode(FString Code);

	UPROPERTY(BlueprintAssignable)
	FStringTransferSignature OnCampaignNameChanged;

	UPROPERTY(BlueprintAssignable)
	FIntegerTransferSignature OnPlayerNumChanged;

	UPROPERTY(BlueprintAssignable)
	FBooleanTransferSignature OnIsSupervisedChanged;

	UPROPERTY(BlueprintAssignable)
	FVoidDataTransferSignature OnSupervisionChangeable;
};
