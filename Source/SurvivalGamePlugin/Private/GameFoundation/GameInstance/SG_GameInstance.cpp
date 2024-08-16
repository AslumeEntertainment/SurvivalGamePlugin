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

bool USG_GameInstance::Login()
{

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::Login IsValid(World) == false"));
		return false;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::Login IsValid(Subsystem) == false"));
		return false;
	}

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (Identity == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::Login IsValid(Identity) == false"));
		return false;
	}

	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return true;
	}

	LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(0,FOnLoginComplete::FDelegate::CreateUObject(this,&USG_GameInstance::HandleLoginComplete));

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
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleLoginComplete IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleLoginComplete IsValid(Subsystem) == false"));
		return;
	}

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (Identity == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleLoginComplete IsValid(Identity) == false"));
		return;
	}
	Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, this->LoginDelegateHandle);

	LoginDelegateHandle.Reset();

	if (!bWasSuccessful)
	{
		UE_LOG(Networking, Warning, TEXT("EOS login failed Error: %s"), *Error);
		return;
		
	}
	UE_LOG(Networking, Display, TEXT("Login callback completed!"));

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::CreateSession IsValid(Session) == false"));
		return;
	}

	SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &USG_GameInstance::OnSessionUserInviteAccepted);

}

bool USG_GameInstance::CreateSession()
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::CreateSession IsValid(World) == false"));
		return false;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::CreateSession IsValid(Subsystem) == false"));
		return false;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::CreateSession IsValid(Session) == false"));
		return false;
	}

	CreateSessionDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::CreateUObject(this, &USG_GameInstance::HandleCreateSessionComplete));


	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bShouldAdvertise = false; // Set to true to make this session discoverable with FindSessions.
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = false;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinInProgress = false;

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

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleCreateSessionComplete IsValid(Subsystem) == false"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleCreateSessionComplete IsValid(Session) == false"));
		return;
	}
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	CreateSessionDelegateHandle.Reset();

	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Warning, TEXT("Cloud NOT create session"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Session Created"));

	World->ServerTravel(LobbyMapName+ "?listen");
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

	UWorld* World = InPlayerController->GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::RegisterNewPlayer IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::RegisterNewPlayer IsValid(Subsystem) == false"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::RegisterNewPlayer IsValid(Session) == false"));
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

	if (!SessionInterface->RegisterPlayer(GameSessionLocalName, *UniqueNetId, true))
	{
		// The player could not be registered; typically you will want to kick the player from the server in this situation.
		UE_LOG(Networking, Error, TEXT("New player could NOT be reghistered"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("New Player Registered Successfully"));

	AGameplayGameMode* GameMode = World->GetAuthGameMode<AGameplayGameMode>();

	if (IsValid(GameMode) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::RegisterNewPlayer IsValid(GameMode) == false check the settings to ensure that the GameMode is derived form AGameplayGameMode"));
		return;
	}
	GameMode->JoinTeam(InPlayerController, ETeam::Teamless);

}

void USG_GameInstance::UnRegisterPlayer(APlayerController* InPlayerController)
{
	UE_LOG(Networking, Display, TEXT("UnRegistering player..."));

	UWorld* World = InPlayerController->GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::UnRegisterPlayer IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::UnRegisterPlayer IsValid(Subsystem) == false"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::UnRegisterPlayer IsValid(Session) == false"));
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

	if (!SessionInterface->UnregisterPlayer(GameSessionLocalName, *UniqueNetId))
	{
		// The player could not be registered; typically you will want to kick the player from the server in this situation.
		UE_LOG(Networking, Error, TEXT("Player could NOT be unreghistered"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Player Unegistered Successfully"));

	AGameplayGameMode* GameMode = World->GetAuthGameMode<AGameplayGameMode>();

	if (IsValid(GameMode) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::RegisterNewPlayer IsValid(GameMode) == false check the settings to ensure that the GameMode is derived form AGameplayGameMode"));
		return;
	}
	ETeam TeamToLeave = GameMode->GetCurrentTeam(InPlayerController);
	GameMode->DestroyPlayerCustomiser(InPlayerController, TeamToLeave);
}

void USG_GameInstance::OnSessionUserInviteAccepted(bool bWasSuccessful, int ControllerId, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UserId, const FOnlineSessionSearchResult& OnlineSessionSearchResult)
{

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::OnSessionUserInviteAccepted IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::OnSessionUserInviteAccepted IsValid(Subsystem) == false"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::OnSessionUserInviteAccepted IsValid(Session) == false"));
		return;
	}

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

	AbandonLocalSession();
}

bool USG_GameInstance::AbandonLocalSession()
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::DestroySession IsValid(World) == false"));
		return false;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::DestroySession IsValid(Subsystem) == false"));
		return false;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::DestroySession IsValid(Session) == false"));
		return false;
	}

	AbandonLocalSessionDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionComplete::FDelegate::CreateUObject(this, &USG_GameInstance::HandleAbandonLocalSessionComplete));

	bool bSuccess = SessionInterface->DestroySession(GameSessionLocalName);
	if (!bSuccess)
	{
		UE_LOG(Networking, Warning, TEXT("Failed to initiate session abandoning........Proceeding anyway"));
		HandleAbandonLocalSessionComplete("NoSession", true);
		return false;
	}
	UE_LOG(Networking, Display, TEXT("Abandoning session..."));

	return true;
}

void USG_GameInstance::HandleAbandonLocalSessionComplete(FName SessionName, bool bWasSuccessful)
{

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleDestroySessionComplete IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleDestroySessionComplete IsValid(Subsystem) == false"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleDestroySessionComplete IsValid(Session) == false"));
		return;
	}

	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(AbandonLocalSessionDelegateHandle);
	AbandonLocalSessionDelegateHandle.Reset();

	if (bWasSuccessful == false)
	{
		//fire off error delegate for this shit
		UE_LOG(Networking, Warning, TEXT("Cloud NOT abandon session"));
		return;
	}

	UE_LOG(Networking, Display, TEXT("Session Abandoned - Joining server session"));

	JoinSessionDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionComplete::FDelegate::CreateUObject(this, &USG_GameInstance::HandleJoinSessionComplete));

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

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleJoinSessionComplete IsValid(Subsystem) == false"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

	if (SessionInterface == nullptr)
	{
		UE_LOG(Networking, Error, TEXT("USG_GameInstance::HandleJoinSessionComplete IsValid(Session) == false"));
		return;
	}

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionDelegateHandle);
	JoinSessionDelegateHandle.Reset();

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

	PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);//add ,true for seamless travel

}

