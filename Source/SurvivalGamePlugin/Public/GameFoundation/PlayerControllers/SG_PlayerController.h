// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Headers/GeneralDelegates.h"

#include "SG_PlayerController.generated.h"


class USG_SaveGame;

UENUM(BlueprintType)
enum class EInputMode : uint8
{
	GameOnly,
	UIOnly,
	GameAndUI,
};

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY()
	USG_SaveGame* Save = nullptr;

	UPROPERTY(EditAnywhere, Category = "Developer")
	bool bIsCameraMovementConstrained = true;
	
	UFUNCTION()
	void ReceiveReadSave(USaveGame* SaveGame);

public:

	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_SetInputMode(EInputMode InputMode);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_LeaveSession(bool bWasKicked=false);

	void OnNetCleanup(UNetConnection* Connection) override;

	void OnPossess(APawn* InPawn) override;

	//Work with save game
	UFUNCTION()
	void ReadSave();

	UFUNCTION()
	void WriteSave();

	USG_SaveGame* GetSave();

	FPlayerControllerTransferSignature OnConnectionCleanUp;

	FVoidDataTransferSignature OnReadSaveRequested;
	FSaveGameTransferSignature OnWriteSaveRequested;

	FVoidDataTransferSignature OnSaveUpdated;


	//----------------Execs--------------------------

	UFUNCTION(Exec)
	void Cheat_SetMoney(uint32 Money);
};
