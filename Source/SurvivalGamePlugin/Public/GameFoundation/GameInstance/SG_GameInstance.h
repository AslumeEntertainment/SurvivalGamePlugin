// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"


#include "OnlineSessionSettings.h" 

#include "SG_GameInstance.generated.h"


/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API USG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	FDelegateHandle LoginDelegateHandle;

	void HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	FDelegateHandle CreateSessionDelegateHandle;

	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void RegisterExistingPlayers();

	FDelegateHandle AbandonLocalSessionDelegateHandle;

	void HandleAbandonLocalSessionComplete(FName SessionName, bool bWasSuccessful);

	virtual void OnSessionUserInviteAccepted(bool bWasSuccessful, int ControllerId, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UserId, const FOnlineSessionSearchResult& OnlineSessionSearchResult);

	FDelegateHandle JoinSessionDelegateHandle;

	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);

	FString ConnectInfo = "";
	FOnlineSessionSearchResult CurrentSessionToJoin;

public:

	UPROPERTY(EditAnywhere)
	FString LobbyMapName = "ThirdPersonMap";

	UPROPERTY(EditAnywhere)
	FName GameSessionLocalName = "GameSessionLocalName";
	
/*
	UFUNCTION(BlueprintCallable)
	bool IsPlayerLoggedIn();

	UFUNCTION(BlueprintCallable)
	FString GetLocalPlayerUsername();
*/
	
	UFUNCTION(BlueprintCallable)
	bool Login();

	UFUNCTION(BlueprintCallable)
	bool CreateSession();

	UFUNCTION(BlueprintCallable)
	bool AbandonLocalSession();

	UFUNCTION(BlueprintCallable)
	void RegisterNewPlayer(APlayerController* InPlayerController);

};