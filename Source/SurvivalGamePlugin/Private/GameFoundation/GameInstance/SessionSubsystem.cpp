// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/SessionSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h" 

DEFINE_LOG_CATEGORY_STATIC(SessionSubsystem, Log, All);

void USessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("Initialize IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(World);
	if (OnlineSubsystem == nullptr)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("Initialize IsValid(OnlineSubsystem) == false"));
		return;
	}

	SessionInterface = OnlineSubsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("Initialize IsValid(SessionInterface) == false"));
	}

	SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &USessionSubsystem::OnSessionUserInviteAccepted);
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USessionSubsystem::HandleCreateSessionComplete);
	SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USessionSubsystem::HandleDestroySessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USessionSubsystem::HandleJoinSessionComplete);

	LocalSessionState = ESessionState::Idle;
}

void USessionSubsystem::Deinitialize()
{
	SessionInterface->ClearOnSessionUserInviteAcceptedDelegates(this);
	SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
	SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
	SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
	SessionInterface = nullptr;
}

bool USessionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (GetClass()->IsInBlueprint() && Super::ShouldCreateSubsystem(Outer))
	{
		return true;
	}
	return false;
}

void USessionSubsystem::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("HandleCreateSessionComplete IsValid(World) == false"));
		return;
	}

	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Warning, TEXT("Cloud NOT create session"));
		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("Session Created"));
	//check if work without hsit line
	World->ServerTravel(LobbyMapName + "?listen");

	LocalSessionState = ESessionState::Hosting;

	RegisterExistingPlayers();
}

void USessionSubsystem::RegisterExistingPlayers()
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("RegisterExistingPlayers IsValid(World) == false"));
		return;
	}

	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();

		RegisterNewPlayer(PlayerController);
	}
}

void USessionSubsystem::HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{

	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Warning, TEXT("Cloud NOT abandon session"));
		return;
	}

	if (bShouldStayIdle)
	{
		UWorld* World = GetWorld();

		if (IsValid(World) == false)
		{
			UE_LOG(SessionSubsystem, Error, TEXT("DestroySession IsValid(World) == false"));
			return;
		}

		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		if (IsValid(PC) == false)
		{
			UE_LOG(SessionSubsystem, Error, TEXT("DestroySession IsValid(PC) == false"));
			return;
		}

		CurrentSessionToJoin = FOnlineSessionSearchResult();
		ConnectInfo = "";

		UE_LOG(LogTemp, Display, TEXT("SERVER: CLIENT TRAVEL TO IDLE STATE"));
		PC->ClientTravel(LobbyMapName, ETravelType::TRAVEL_Absolute);

		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("Session Abandoned - Joining server session"));

	bool bSuccess = SessionInterface->JoinSession(0, GameSessionLocalName, CurrentSessionToJoin);
	if (!bSuccess)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Error, TEXT("Failed to initiate joining session..."));
		return;
	}
	UE_LOG(SessionSubsystem, Display, TEXT("Joining session..."));

}

void USessionSubsystem::OnSessionUserInviteAccepted(bool bWasSuccessful, int ControllerId, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UserId, const FOnlineSessionSearchResult& OnlineSessionSearchResult)
{
	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Warning, TEXT("Invite was NOT accepted properly"));
		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("Invite Accepted"));

	bool bSuccess = SessionInterface->GetResolvedConnectString(OnlineSessionSearchResult, NAME_GamePort, ConnectInfo);
	if (!bSuccess)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Error, TEXT("Failed to to resolve connection string"));
		return;
	}

	CurrentSessionToJoin = OnlineSessionSearchResult;

	if (LocalSessionState != ESessionState::Idle)
	{
		DestroySession(false);
		return;
	}

	bSuccess = SessionInterface->JoinSession(0, GameSessionLocalName, CurrentSessionToJoin);
	if (!bSuccess)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Error, TEXT("Failed to initiate joining session..."));
		return;
	}
	UE_LOG(SessionSubsystem, Display, TEXT("Joining session..."));
}

void USessionSubsystem::HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("HandleJoinSessionComplete IsValid(World) == false"));
		return;
	}
	//establish network error and travel error delegate handle 

	bool bIsIn = (JoinResult == EOnJoinSessionCompleteResult::Success || JoinResult == EOnJoinSessionCompleteResult::AlreadyInSession);

	if (!bIsIn)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("Could NOT join session"));
		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("Session joined  -  traveling to server"));


	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (IsValid(PC) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("HandleJoinSessionComplete IsValid(PC) == false"));
		return;
	}

	LocalSessionState = ESessionState::Joined;

	//test if it will work without this line
	PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);//add ,true for seamless travel

}

bool USessionSubsystem::CreateSession(int NumPublicConnections, bool bShouldAdvertise, bool bAllowInvites, bool bAllowJoinViaPresence, bool bAllowJoinInProgress)
{
	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = NumPublicConnections;
	SessionSettings->bShouldAdvertise = bShouldAdvertise; // Set to true to make this session discoverable with FindSessions.
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = false;
	SessionSettings->bAllowInvites = bAllowInvites;
	SessionSettings->bAllowJoinViaPresence = bAllowJoinViaPresence;
	SessionSettings->bAllowJoinInProgress = bAllowJoinInProgress;

	// You *must* set at least one setting value, because you can not run FindSessions without any filters.
	SessionSettings->Settings.Add(
		FName(TEXT("SessionSetting")),
		FOnlineSessionSetting(FString(TEXT("SettingValue")), EOnlineDataAdvertisementType::ViaOnlineService));

	// Create a session and give the local name "MyLocalSessionName". This name is entirely local to the current player and isn't stored in EOS.
	bool bSuccess = SessionInterface->CreateSession(0, GameSessionLocalName, *SessionSettings);
	if (!bSuccess)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("Failed to initiate session..."));
		return false;
	}
	UE_LOG(SessionSubsystem, Display, TEXT("Initiating session..."));

	return true;
}

void USessionSubsystem::DestroySession(bool InbShouldStayIdle)
{
	bShouldStayIdle = InbShouldStayIdle;

	ESessionState PrevState = LocalSessionState;

	LocalSessionState = ESessionState::Idle;

	if (PrevState == ESessionState::Hosting)
	{
		OnHostedGameClosing.Broadcast();
	}

	bool bSuccess = SessionInterface->DestroySession(GameSessionLocalName);
	if (!bSuccess)
	{
		UE_LOG(SessionSubsystem, Warning, TEXT("Failed to initiate session abandoning........"));
		return;
	}
	UE_LOG(SessionSubsystem, Display, TEXT("Abandoning session..."));
}

void USessionSubsystem::RegisterNewPlayer(APlayerController* InPlayerController)
{
	UE_LOG(SessionSubsystem, Display, TEXT("Registering new player..."));

	if (LocalSessionState != ESessionState::Hosting)
	{
		UE_LOG(SessionSubsystem, Display, TEXT("SESSION ALREADY CLOSED"));
		return;
	}

	FUniqueNetIdRepl UniqueNetIdRepl;
	if (InPlayerController->IsLocalPlayerController())
	{
		ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			LocalUniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
			UniqueNetIdRepl = LocalUniqueNetIdRepl;
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
			check(IsValid(RemoteNetConnection));
			LocalUniqueNetIdRepl = RemoteNetConnection->PlayerId;
			UniqueNetIdRepl = LocalUniqueNetIdRepl;
		}
	}
	else
	{
		UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	check(UniqueNetId != nullptr);

	if (SessionInterface->IsPlayerInSession(GameSessionLocalName, *UniqueNetId))
	{
		UE_LOG(SessionSubsystem, Display, TEXT("New Player Already Registered"));
		return;
	}

	if (!SessionInterface->RegisterPlayer(GameSessionLocalName, *UniqueNetId, true))
	{
		// The player could not be registered; typically you will want to kick the player from the server in this situation.
		UE_LOG(SessionSubsystem, Error, TEXT("New player could NOT be reghistered"));
		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("New Player Registered Successfully"));
}

void USessionSubsystem::UnRegisterPlayer(APlayerController* InPlayerController)
{
	UE_LOG(SessionSubsystem, Display, TEXT("UnRegistering player..."));

	if (LocalSessionState != ESessionState::Hosting)
	{
		UE_LOG(SessionSubsystem, Display, TEXT("SESSION ALREADY CLOSED"));
		return;
	}

	FUniqueNetIdRepl UniqueNetIdRepl;
	if (InPlayerController->IsLocalPlayerController())
	{
		ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	else
	{
		UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	check(UniqueNetId != nullptr);

	if (!SessionInterface->IsPlayerInSession(GameSessionLocalName, *UniqueNetId))
	{
		UE_LOG(SessionSubsystem, Display, TEXT("New Player Already Unregistered"));
		return;
	}

	if (!SessionInterface->UnregisterPlayer(GameSessionLocalName, *UniqueNetId))
	{
		// The player could not be registered; typically you will want to kick the player from the server in this situation.
		UE_LOG(SessionSubsystem, Error, TEXT("Player could NOT be unreghistered"));
		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("Player Unegistered Successfully"));
}

void USessionSubsystem::InviteFriendToSession(FUniqueNetIdRef FriendNetIdPtr)
{
	SessionInterface->SendSessionInviteToFriend(*LocalUniqueNetIdRepl.GetUniqueNetId(), GameSessionLocalName, *FriendNetIdPtr);
}
