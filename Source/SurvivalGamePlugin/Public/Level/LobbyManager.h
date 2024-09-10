// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headers/GeneralDelegates.h"
#include "LobbyManager.generated.h"

class AGameManager;

UCLASS()
class SURVIVALGAMEPLUGIN_API ALobbyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyManager();

private:

	int NumPlayers = 4;
	AGameManager* ActiveGameManager = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldPrintdDebug = true;

	UPROPERTY(EditAnywhere, Category = "GameManaging")
	TMap<int, AGameManager*> TablesByPlayerNum;

public:

	UFUNCTION()
	void SetNumPlayers(int InNumPlayers);

	UFUNCTION(BlueprintCallable)//remove this tag
	void CreateLobby();

	UFUNCTION(BlueprintCallable)
	void LeaveLobby();

};
