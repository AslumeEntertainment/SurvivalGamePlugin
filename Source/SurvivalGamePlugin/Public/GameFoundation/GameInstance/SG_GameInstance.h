// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Headers/SessionState.h"

#include "OnlineSessionSettings.h" 

#include "SG_GameInstance.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FHostedGameClosing);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFreindListSignature, TArray<FString>, FriendNames);

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	bool bShouldStayIdle = true;

	IOnlineIdentityPtr Identity = nullptr;
	IOnlineSessionPtr SessionInterface = nullptr;
	IOnlineFriendsPtr FriendsInterface = nullptr;

	void HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void RegisterExistingPlayers();

	void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	virtual void OnSessionUserInviteAccepted(bool bWasSuccessful, int ControllerId, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UserId, const FOnlineSessionSearchResult& OnlineSessionSearchResult);

	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);

	FString ConnectInfo = "";
	FOnlineSessionSearchResult CurrentSessionToJoin;

	void HandleReadFriendListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	TMap<FString, FUniqueNetIdRef> FriendList;

	void HandleSessionFailure(const FUniqueNetId& PlayerId, ESessionFailure::Type FailureType);
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

public:

	void OnStart() override;

	ESessionState LocalSessionState = ESessionState::Idle;

	UPROPERTY(EditAnywhere)
	FString LobbyMapName = "ThirdPersonMap";

	//UPROPERTY(EditAnywhere)
	//FString DefaultGameMapName = "TestWorld";

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
	bool CreateSession(bool bIsLANMatch = false, int NumPublicConnections = 4, bool bShouldAdvertise = false, bool bAllowInvites = true, bool bAllowJoinViaPresence = true, bool bAllowJoinInProgress = false);

	UFUNCTION(BlueprintCallable)
	void DestroySession(bool bShouldStayIdle = true);

	UFUNCTION(BlueprintCallable)
	void RegisterNewPlayer(APlayerController* InPlayerController);

	UFUNCTION(BlueprintCallable)
	void UnRegisterPlayer(APlayerController* InPlayerController);

	UFUNCTION(BlueprintCallable)
	void ReadFriendList();

	UFUNCTION(BlueprintCallable)
	void InviteFriendToSession(FString FriendName);

	FHostedGameClosing OnHostedGameClosing;
	FFreindListSignature OnFreindListUpdated;
};