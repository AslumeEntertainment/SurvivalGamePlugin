// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/SG_GameInstance.h"

#include "Kismet/GameplayStatics.h" 
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "GameFoundation/GameModes/GameplayGameMode.h"

DEFINE_LOG_CATEGORY_STATIC(Networking, Log, All);

void USG_GameInstance::OnStart()
{
	Super::OnStart();

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::Login IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::Login IsValid(Subsystem) == false"));
		return;
	}

	Identity = Subsystem->GetIdentityInterface();
	SessionInterface = Subsystem->GetSessionInterface();
	FriendsInterface = Subsystem->GetFriendsInterface();

	if (Identity == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::Login IsValid(Identity) == false"));
		return;
	}

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::Login IsValid(Session) == false"));
		return;
	}

	if (FriendsInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::ReadFriendList IsValid(FriendsInterface) == false"));
		return;
	}

	Identity->OnLoginCompleteDelegates->AddUObject(this, &USG_GameInstance::HandleLoginComplete);
	SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &USG_GameInstance::OnSessionUserInviteAccepted);
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USG_GameInstance::HandleCreateSessionComplete);
	SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USG_GameInstance::HandleDestroySessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USG_GameInstance::HandleJoinSessionComplete);
	SessionInterface->OnSessionFailureDelegates.AddUObject(this, &USG_GameInstance::HandleSessionFailure);

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &USG_GameInstance::HandleNetworkFailure);
	}

}

bool USG_GameInstance::Login()
{

	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return true;
	}

	//LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(0,FOnLoginComplete::FDelegate::CreateUObject(this,&USG_GameInstance::HandleLoginComplete));

	if (!Identity->AutoLogin(0 /* LocalUserNum */))
	{
		UE_LOG(Networking, Warning, TEXT("Could not start auto log in for local user..."));

		FOnlineAccountCredentials Credentials;
		Credentials.Id = "";
		Credentials.Token = "";
		Credentials.Type = "accountportal";

		if (!Identity->Login(0, Credentials))
		{
			UE_LOG(Networking, Error, TEXT("Could not start forced log in for local user..."));
			return false;
		}
	}
	UE_LOG(Networking, Display, TEXT("Started loggin in..."));
	return true;
}

void USG_GameInstance::HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{

	Identity->ClearOnLoginCompleteDelegates(0,this);

	if (!bWasSuccessful)
	{
		UE_LOG(Networking, Warning, TEXT("EOS login failed Error: %s"), *Error);
		return;
		
	}
	UE_LOG(Networking, Display, TEXT("Login callback completed!"));

	LocalSessionState = ESessionState::Idle;
}

bool USG_GameInstance::CreateSession(int NumPublicConnections, bool bShouldAdvertise, bool bAllowInvites, bool bAllowJoinViaPresence, bool bAllowJoinInProgress)
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
		UE_LOG(Networking, Error, TEXT("Failed to initiate session..."));
		return false;
	}
	UE_LOG(Networking, Display, TEXT("Initiating session..."));

	return true;
}

void USG_GameInstance::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleCreateSessionComplete IsValid(World) == false"));
		return;
	}

	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Warning, TEXT("Cloud NOT create session"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Session Created"));
	//check if work without hsit line
	World->ServerTravel(LobbyMapName+ "?listen");

	LocalSessionState = ESessionState::Hosting;

	RegisterExistingPlayers();

}

void USG_GameInstance::RegisterExistingPlayers()
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::RegisterExistingPlayers IsValid(World) == false"));
		return;
	}

	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();

		RegisterNewPlayer(PlayerController);
	}

	//this->bAllExistingPlayersRegistered = true;
}

void USG_GameInstance::RegisterNewPlayer(APlayerController* InPlayerController)
{
	UE_LOG(Networking, Display, TEXT("Registering new player..."));

	if (LocalSessionState != ESessionState::Hosting)
	{
		UE_LOG(Networking, Display, TEXT("SESSION ALREADY CLOSED"));
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

	if (SessionInterface->IsPlayerInSession(GameSessionLocalName, *UniqueNetId))
	{
		UE_LOG(Networking, Display, TEXT("New Player Already Registered"));
		return;
	}

	if (!SessionInterface->RegisterPlayer(GameSessionLocalName, *UniqueNetId, true))
	{
		// The player could not be registered; typically you will want to kick the player from the server in this situation.
		UE_LOG(Networking, Error, TEXT("New player could NOT be reghistered"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("New Player Registered Successfully"));

}

void USG_GameInstance::UnRegisterPlayer(APlayerController* InPlayerController)
{
	UE_LOG(Networking, Display, TEXT("UnRegistering player..."));

	if (LocalSessionState != ESessionState::Hosting)
	{
		UE_LOG(Networking, Display, TEXT("SESSION ALREADY CLOSED"));
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
		UE_LOG(Networking, Display, TEXT("New Player Already Unregistered"));
		return;
	}

	if (!SessionInterface->UnregisterPlayer(GameSessionLocalName, *UniqueNetId))
	{
		// The player could not be registered; typically you will want to kick the player from the server in this situation.
		UE_LOG(Networking, Error, TEXT("Player could NOT be unreghistered"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Player Unegistered Successfully"));

}

void USG_GameInstance::OnSessionUserInviteAccepted(bool bWasSuccessful, int ControllerId, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UserId, const FOnlineSessionSearchResult& OnlineSessionSearchResult)
{
	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Warning, TEXT("Invite was NOT accepted properly"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Invite Accepted"));

	bool bSuccess = SessionInterface->GetResolvedConnectString(OnlineSessionSearchResult, NAME_GamePort, ConnectInfo);
	if (!bSuccess)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Error, TEXT("Failed to to resolve connection string"));
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
		UE_LOG(Networking, Error, TEXT("Failed to initiate joining session..."));
		return;
	}
	UE_LOG(Networking, Display, TEXT("Joining session..."));
	
}

void USG_GameInstance::DestroySession(bool InbShouldStayIdle)
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
		UE_LOG(Networking, Warning, TEXT("Failed to initiate session abandoning........"));
		return;
	}
	UE_LOG(Networking, Display, TEXT("Abandoning session..."));
}

void USG_GameInstance::HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{

	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Warning, TEXT("Cloud NOT abandon session"));
		return;
	}

	if (bShouldStayIdle)
	{
		UWorld* World = GetWorld();

		if (IsValid(World) == false)
		{
			UE_LOG(Networking, Error, TEXT("USG_GameInstance::DestroySession IsValid(World) == false"));
			return;
		}

		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		if (IsValid(PC) == false)
		{
			UE_LOG(Networking, Error, TEXT("USG_GameInstance::DestroySession IsValid(PC) == false"));
			return;
		}

		CurrentSessionToJoin = FOnlineSessionSearchResult();
		ConnectInfo = "";

		UE_LOG(LogTemp, Display, TEXT("SERVER: CLIENT TRAVEL TO IDLE STATE"));
		PC->ClientTravel(LobbyMapName, ETravelType::TRAVEL_Absolute);

		return;
	}

	UE_LOG(Networking, Display, TEXT("Session Abandoned - Joining server session"));

	bool bSuccess = SessionInterface->JoinSession(0, GameSessionLocalName, CurrentSessionToJoin);
	if (!bSuccess)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Error, TEXT("Failed to initiate joining session..."));
		return;
	}
	UE_LOG(Networking, Display, TEXT("Joining session..."));

}

void USG_GameInstance::HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleJoinSessionComplete IsValid(World) == false"));
		return;
	}
	//establish network error and travel error delegate handle 

	bool bIsIn = (JoinResult == EOnJoinSessionCompleteResult::Success || JoinResult == EOnJoinSessionCompleteResult::AlreadyInSession);

	if (!bIsIn)
	{
		UE_LOG(Networking, Error, TEXT("Could NOT join session"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Session joined  -  traveling to server"));


	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (IsValid(PC) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleJoinSessionComplete IsValid(PC) == false"));
		return;
	}

	LocalSessionState = ESessionState::Joined;

	//test if it will work without this line
	PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);//add ,true for seamless travel

}

void USG_GameInstance::ReadFriendList()
{
	bool bSuccess = FriendsInterface->ReadFriendsList(
		0 /* LocalUserNum */,
		TEXT("") /* ListName, unused by EOS */,
		FOnReadFriendsListComplete::CreateUObject(this, &USG_GameInstance::HandleReadFriendListComplete)
	);

	if (!bSuccess)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Error, TEXT("Failed to start reading friends list..."));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Started reading friends list..."));

}

void USG_GameInstance::HandleReadFriendListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	if (!bWasSuccessful)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Error, TEXT("Failed to read friends list..."));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Read friends list successfully"));

	TArray<TSharedRef<FOnlineFriend>> FriendsArr;
	FriendsInterface->GetFriendsList(
		0 /* LocalUserNum */,
		TEXT("") /* ListName, unused by EOS */,
		FriendsArr /* OutFriends */
	);
	for (const auto& Friend : FriendsArr)
	{
		// Access the friend via Friend.
		FriendList.Add(Friend->GetDisplayName(), Friend->GetUserId());
	}

	TArray<FString> FriendNames;
	FriendList.GenerateKeyArray(FriendNames);

	OnFreindListUpdated.Broadcast(FriendNames);

}

void USG_GameInstance::InviteFriendToSession(FString FriendName)
{
	
	FUniqueNetIdRepl LocalUniqueNetIdRepl;
	APlayerController* PlayerController = GetFirstLocalPlayerController();

	if (PlayerController->IsLocalPlayerController())
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			LocalUniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerController->Player);
			check(IsValid(RemoteNetConnection));
			LocalUniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	
	FUniqueNetIdRef* FriendNetIdPtr = FriendList.Find(FriendName);
	if (FriendNetIdPtr == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::InviteFriendToSession FriendNetIdPtr == nullptr No friend with name: %s"), *FriendName);
		return;
	}

	SessionInterface->SendSessionInviteToFriend(*LocalUniqueNetIdRepl.GetUniqueNetId(), GameSessionLocalName, **FriendNetIdPtr);
}

//For some reason this is never called may remove in the future
void USG_GameInstance::HandleSessionFailure(const FUniqueNetId& PlayerId, ESessionFailure::Type FailureType)
{
	UE_LOG(Networking, Error, TEXT("--------------------------------------------------SESSION FAILURE-----------------------------------------"));
}

void USG_GameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(Networking, Warning, TEXT("--------------------------------------------------NETWORK FAILURE-----------------------------------------"));
	UE_LOG(Networking, Error, TEXT("Error: %s") ,  *ErrorString);
	//fire of networking error delegate
}