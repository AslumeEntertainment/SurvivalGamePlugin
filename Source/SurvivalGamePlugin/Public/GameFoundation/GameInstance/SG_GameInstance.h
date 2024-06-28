// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "VoiceChat.h"

#include "SG_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API USG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	//Callback function. This function is ran when signing into EOS Game Services completes. 
	void HandleLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

public:

	UFUNCTION(BlueprintCallable)
	bool IsPlayerLoggedIn();

	UFUNCTION(BlueprintCallable)
	FString GetLocalPlayerUsername();


	UFUNCTION(BlueprintCallable)
	bool Login(FString ID, FString Token, FString AuthType);

};