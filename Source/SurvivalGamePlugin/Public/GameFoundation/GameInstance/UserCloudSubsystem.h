// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Interfaces/OnlineUserCloudInterface.h"

#include "Headers/GeneralDelegates.h"

#include "UserCloudSubsystem.generated.h"


UCLASS(Blueprintable)
class SURVIVALGAMEPLUGIN_API UUserCloudSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	IOnlineUserCloudPtr UserCloudInterface = nullptr;

	void Initialize(FSubsystemCollectionBase& Collection) override;

	void Deinitialize() override;

	bool ShouldCreateSubsystem(UObject* Outer) const override;

	void HandleReadSaveFileComplete(bool bWasSuccessful, const FUniqueNetId& UserId, const FString& FileName);

	void HandleWriteSaveFileComplete(bool bWasSuccessful, const FUniqueNetId&, const FString& FileName);

public:

	UPROPERTY(EditDefaultsOnly)
	FString SaveFileName = "GeneralSaveFile";

	UFUNCTION()
	void ReadSaveFile();

	UFUNCTION()
	void WriteSaveFile(USaveGame* GameToSave);

	FSaveGameTransferSignature OnGameRead;
	FVoidDataTransferSignature OnWriteFinished;
	FStringTransferSignature OnError;
};
