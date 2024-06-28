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

	//Callback function. This function is ran when signing into EOS Game Services completes. 
	void HandleLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	//Delegate to bind callback event for login. 
	FDelegateHandle LoginDelegateHandle;
	FName LobbyName = "LobbyName";

	UPROPERTY(EditAnywhere)
	FString GameMap = "OverWorld";

	UPROPERTY(EditAnywhere)
	FString HostingMap = "Hosting";

	FString ConnectString;
	FOnlineSessionSearchResult* LobbyToJoin;

	// Function to create an EOS session

	void HandleCreateLobbyCompleted(FName EOSLobbyName, bool bWasSuccessful);

	FDelegateHandle CreateLobbyDelegateHandle;

	void SetupNotifications();

	void HandleParticipantChanged(FName EOSLobbyName, const FUniqueNetId& NetId, bool bJoined);


	void HandleFindLobbiesCompleted(bool bWasSuccessful, TSharedRef<class FOnlineSessionSearch> Search);
	FDelegateHandle FindLobbiesDelegateHandle;

	void JoinLobby();
	void HandleJoinLobbyCompleted(FName LobbyName, EOnJoinSessionCompleteResult::Type Result);
	FDelegateHandle JoinLobbyDelegateHandle;

	FDelegateHandle StartLobbyDelegateHandle;
	void HandleStartLobbyCompleted(FName LobbyName, bool bWasSuccessful);

	FString GenerateSessionCode();

public:

	FString HostingMode = "";

	UFUNCTION(BlueprintCallable)
	bool Login(FString ID, FString Token, FString AuthType);

	UFUNCTION()
	FString CreateLobby(FName KeyName);

	UFUNCTION()
	bool FindLobbies(FName SearchKey, FString SearchValue);

	UFUNCTION()
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void ToggleVoiceChat(bool State);
};