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

	bool bShouldCloseGame = false;

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
	FString MainMenuMap = "EmptyMapo";


	UPROPERTY(EditDefaultsOnly, Category = "ServerSettings")
	FName GameSessionLocalName = "GameSessionLocalName";

	UPROPERTY(EditDefaultsOnly, Category = "ServerSettings")
	int ConnectionPort = 7779;

	UPROPERTY(EditDefaultsOnly, Category = "SessionSettings")
	int NumPublicConnections = 8;

	UPROPERTY(EditDefaultsOnly, Category = "SessionSettings")
	bool bIsLANMatch = false;

	UPROPERTY(EditDefaultsOnly, Category = "SessionSettings")
	bool bShouldAdvertise = false;

	UPROPERTY(EditDefaultsOnly, Category = "SessionSettings")
	bool bUsesPresence = true;

	UPROPERTY(EditDefaultsOnly, Category = "SessionSettings")
	bool bUseLobbiesIfAvailable = false;

	UPROPERTY(EditDefaultsOnly, Category = "SessionSettings")
	bool bAllowInvites = true;

	UPROPERTY(EditDefaultsOnly, Category = "SessionSettings")
	bool bAllowJoinViaPresence = false;

	UPROPERTY(EditDefaultsOnly, Category = "SessionSettings")
	bool bAllowJoinInProgress = false;

	
	UFUNCTION(BlueprintCallable)
	bool CreateSession();

	UFUNCTION(BlueprintCallable)
	void DestroySession(bool InbShouldStayIdle = true, bool InShouldCloseGame = false);

	UFUNCTION(BlueprintCallable)
	void RegisterNewPlayer(APlayerController* InPlayerController);

	UFUNCTION(BlueprintCallable)
	void UnRegisterPlayer(APlayerController* InPlayerController);

	void InviteFriendToSession(FUniqueNetIdRef FriendNetIdPtr);

	//UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FVoidDataTransferSignature OnHostedGameClosing;

	UPROPERTY(BlueprintAssignable)
	FStringTransferSignature OnCoverUpWidgetNeeded;

	UPROPERTY(BlueprintAssignable)
	FVoidDataTransferSignature OnCoverUpWidgetNotNeeded;
};
