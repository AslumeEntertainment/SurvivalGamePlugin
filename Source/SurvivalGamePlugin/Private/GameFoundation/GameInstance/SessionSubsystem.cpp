// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/SessionSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h" 

#include "GameFoundation/GameInstance/IdentitySubsystem.h"

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
	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Warning, TEXT("Cloud NOT create session"));
		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("Session Created"));

	LocalSessionState = ESessionState::Hosting;

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("HandleCreateSessionComplete IsValid(World) == false"));
		return;
	}

	UE_LOG(SessionSubsystem, Warning, TEXT("TURNING ON LISTEN SERVER + SERVER TRAVEL"));

	World->ServerTravel(LobbyMapName + "?listen", true);
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

	bool ShouldGoToMainMenu = bShouldCloseGame;

	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Warning, TEXT("Cloud NOT abandon session"));
		ShouldGoToMainMenu = true;
	}

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("HandleDestroySessionComplete IsValid(World) == false"));
		return;
	}

	UE_LOG(SessionSubsystem, Warning, TEXT("TURNING OFF LISTEN SERVER"));
	LocalSessionState = ESessionState::Idle;
	World->GetGameInstance()->EnableListenServer(false, ConnectionPort);
	//World->ServerTravel(MainMenuMap, true); //e za tva se otnasq dolniq komentar

	if (ShouldGoToMainMenu)
	{
		UE_LOG(SessionSubsystem, Display, TEXT("Session Abandoned - Going to main menu"));
		World->ServerTravel(MainMenuMap, true);// sloji go tuka i otkomentirai tva v dolniq if ako ne raboti sus startin movietata
		return;
	}

	if (bShouldStayIdle)
	{
		UE_LOG(SessionSubsystem, Display, TEXT("Session Abandoned - Hosting local session"));
		World->ServerTravel(TransitionLoadingMap, true);
		CreateSession();
		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("Session Abandoned - Joining server session"));

	JoinSession();
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

	UE_LOG(SessionSubsystem, Display, TEXT("Abandoning current session in order to join new session..."));
	if(LocalSessionState != ESessionState::Idle)
	{
		DestroySession(false);
		return;
	}

	JoinSession();
	
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

	PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);

}

bool USessionSubsystem::CreateSession()
{

	UIdentitySubsystem* IdentitySubsystemPtr = GetGameInstance()->GetSubsystem<UIdentitySubsystem>();
	if (IsValid(IdentitySubsystemPtr) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("CreateSession (IsValid(IdentitySubsystemPtr) == false"));
		return false;
	}

	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->bIsLANMatch = bIsLANMatch;
	SessionSettings->NumPublicConnections = NumPublicConnections;
	SessionSettings->bShouldAdvertise = bShouldAdvertise; // Set to true to make this session discoverable with FindSessions.
	SessionSettings->bUsesPresence = bUsesPresence;
	SessionSettings->bUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
	SessionSettings->bAllowInvites = bAllowInvites;
	SessionSettings->bAllowJoinViaPresence = bAllowJoinViaPresence;
	SessionSettings->bAllowJoinInProgress = bAllowJoinInProgress;

	// You *must* set at least one setting value, because you can not run FindSessions without any filters.
	SessionSettings->Settings.Add(
		FName("HostName"),
		FOnlineSessionSetting(IdentitySubsystemPtr->GetLocalPlayerDisplayName(), EOnlineDataAdvertisementType::ViaOnlineService));

	UE_LOG(SessionSubsystem, Warning, TEXT("CREATING SESSION"));
	bool bSuccess = SessionInterface->CreateSession(0, GameSessionLocalName, *SessionSettings);
	if (!bSuccess)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("Failed to initiate session..."));
		return false;
	}
	UE_LOG(SessionSubsystem, Display, TEXT("Initiating session..."));

	return true;
}

void USessionSubsystem::JoinSession()
{


	bool bSuccess = SessionInterface->JoinSession(0, GameSessionLocalName, CurrentSessionToJoin);
	if (!bSuccess)
	{
		//fire off error delegate for this shit
		UE_LOG(SessionSubsystem, Error, TEXT("Failed to initiate joining session..."));
		return;
	}
	UE_LOG(SessionSubsystem, Display, TEXT("Joining session..."));
}

void USessionSubsystem::DestroySession(bool InbShouldStayIdle, bool InShouldCloseGame)
{

	bShouldStayIdle = InbShouldStayIdle;
	bShouldCloseGame = InShouldCloseGame;

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

	TSharedPtr<const FUniqueNetId> UniqueNetId;
	if (InPlayerController->IsLocalPlayerController())
	{
		UIdentitySubsystem* IdentitySubsystemPtr = GetGameInstance()->GetSubsystem<UIdentitySubsystem>();
		if (IsValid(IdentitySubsystemPtr) == false)
		{
			UE_LOG(SessionSubsystem, Error, TEXT("RegisterNewPlayer (IsValid(IdentitySubsystemPtr) == false"));
			return;
		}
		UE_LOG(SessionSubsystem, Warning, TEXT("Getting The LocalPlayer NetId"));
		UniqueNetId = IdentitySubsystemPtr->GetLocalPlayerNetId();
	}
	else
	{
		UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetId = RemoteNetConnection->PlayerId.GetUniqueNetId();
	}

	if (UniqueNetId == nullptr)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("RegisterNewPlayer UniqueNetId == nullptr of %s"), *InPlayerController->GetName());
		return;
	}

	if (SessionInterface->IsPlayerInSession(GameSessionLocalName, *UniqueNetId))
	{
		UE_LOG(SessionSubsystem, Warning, TEXT("New Player Already Registered"));
		return;
	}

	if (!SessionInterface->RegisterPlayer(GameSessionLocalName, *UniqueNetId, true))
	{
		// The player could not be registered; typically you will want to kick the player from the server in this situation.
		UE_LOG(SessionSubsystem, Error, TEXT("New player could NOT be registered"));
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

	TSharedPtr<const FUniqueNetId> UniqueNetId;
	if (InPlayerController->IsLocalPlayerController())
	{
		UIdentitySubsystem* IdentitySubsystemPtr = GetGameInstance()->GetSubsystem<UIdentitySubsystem>();
		if (IsValid(IdentitySubsystemPtr) == false)
		{
			UE_LOG(SessionSubsystem, Error, TEXT("UnRegisterPlayer (IsValid(IdentitySubsystemPtr) == false"));
			return;
		}

		UniqueNetId = IdentitySubsystemPtr->GetLocalPlayerNetId();
	}
	else
	{
		UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetId = RemoteNetConnection->PlayerId.GetUniqueNetId();
	}

	if (UniqueNetId == nullptr)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("UnregisterNewPlayer UniqueNetId == nullptr of %s"), *InPlayerController->GetName());
		return;
	}

	if (!SessionInterface->IsPlayerInSession(GameSessionLocalName, *UniqueNetId))
	{
		UE_LOG(SessionSubsystem, Display, TEXT("New Player Already Unregistered"));
		return;
	}

	if (!SessionInterface->UnregisterPlayer(GameSessionLocalName, *UniqueNetId))
	{
		// The player could not be registered; typically you will want to kick the player from the server in this situation.
		UE_LOG(SessionSubsystem, Error, TEXT("Player could NOT be unregistered"));
		return;
	}

	UE_LOG(SessionSubsystem, Display, TEXT("Player Unregistered Successfully"));
}

void USessionSubsystem::InviteFriendToSession(FUniqueNetIdRef FriendNetIdPtr)
{
	UIdentitySubsystem* IdentitySubsystemPtr = GetGameInstance()->GetSubsystem<UIdentitySubsystem>();
	if (IsValid(IdentitySubsystemPtr) == false)
	{
		UE_LOG(SessionSubsystem, Error, TEXT("InviteFriendToSession (IsValid(IdentitySubsystemPtr) == false"));
		return;
	}
	
	SessionInterface->SendSessionInviteToFriend(*IdentitySubsystemPtr->GetLocalPlayerNetId(), GameSessionLocalName, *FriendNetIdPtr);
}
