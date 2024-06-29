// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

//#include "Interfaces/OnlinePartyInterface.h"

#include "SG_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API USG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	//void HandleLobbyCreated(const FUniqueNetId& LocalUserId, const TSharedPtr<const FOnlinePartyId>& PartyId, const ECreatePartyCompletionResult Result);
	
	//Callback function. This function is ran when signing into EOS Game Services completes. 

	FDelegateHandle LoginDelegateHandle;

	void HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

public:
/*
	UFUNCTION(BlueprintCallable)
	bool IsPlayerLoggedIn();

	UFUNCTION(BlueprintCallable)
	FString GetLocalPlayerUsername();

	UFUNCTION(BlueprintCallable)
	bool CreateParty();
*/
	UFUNCTION(BlueprintCallable)
	bool Login(FString ID, FString Token, FString AuthType);
};