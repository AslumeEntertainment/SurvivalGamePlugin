// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "Headers/GeneralDelegates.h"

#include "IdentitySubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API UIdentitySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	IOnlineIdentityPtr IdentityInterface = nullptr;

	FUniqueNetIdPtr LocalPlayerNetId;

	void Initialize(FSubsystemCollectionBase& Collection) override;

	void Deinitialize() override;

	void HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

public:

	UFUNCTION(BlueprintCallable)
	bool Login();

	FUniqueNetIdPtr GetLocalPlayerNetId();

	FString GetLocalPlayerDisplayName();

	UPROPERTY(BlueprintAssignable)
	FVoidDataTransferSignature OnLoginComplete;

	FTextTransferSignature OnLoginError;

};
