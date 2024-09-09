// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h" 

#include "Headers/SessionState.h"
#include "Headers/GeneralDelegates.h"

#include "SessionSubsystem.generated.h"

/**
 * 
 */

class FonlineFriend;

UCLASS(Blueprintable)
class SURVIVALGAMEPLUGIN_API USessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	bool bShouldStayIdle = true;

	IOnlineSessionPtr SessionInterface = nullptr;

	FString ConnectInfo = "";

	FOnlineSessionSearchResult CurrentSessionToJoin;

	ESessionState LocalSessionState = ESessionState::Idle;

	void Initialize(FSubsystemCollectionBase& Collection) override;

	void Deinitialize() override;

	bool ShouldCreateSubsystem(UObject* Outer) const override;

	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void RegisterExistingPlayers();

	void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void OnSessionUserInviteAccepted(bool bWasSuccessful, int ControllerId, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UserId, const FOnlineSessionSearchResult& OnlineSessionSearchResult);

	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);

public:

	UPROPERTY(EditDefaultsOnly)
	FString LobbyMapName = "ThirdPersonMap";

	UPROPERTY(EditDefaultsOnly)
	FName GameSessionLocalName = "GameSessionLocalName";

	UFUNCTION(BlueprintCallable)
	bool CreateSession(int NumPublicConnections = 4, bool bShouldAdvertise = false, bool bAllowInvites = true, bool bAllowJoinViaPresence = true, bool bAllowJoinInProgress = false);

	UFUNCTION(BlueprintCallable)
	void DestroySession(bool InbShouldStayIdle = true);

	UFUNCTION(BlueprintCallable)
	void RegisterNewPlayer(APlayerController* InPlayerController);

	UFUNCTION(BlueprintCallable)
	void UnRegisterPlayer(APlayerController* InPlayerController);

	void InviteFriendToSession(FUniqueNetIdRef FriendNetIdPtr);

	FVoidDataTransferSignature OnHostedGameClosing;
};
