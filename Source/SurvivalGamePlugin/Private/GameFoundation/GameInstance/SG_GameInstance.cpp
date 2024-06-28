// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/SG_GameInstance.h"

#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

bool USG_GameInstance::Login(FString ID, FString Token, FString AuthType)
{

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::Login IsValid(World) == false"));
		return false;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (Identity == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::Login IsValid(Identity) == false"));
		return false;
	}

	FUniqueNetIdPtr NetId = Identity->GetUniquePlayerId(0);

	/*Identity->Logout(0);

	if (NetId != nullptr && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		UE_LOG(LogTemp, Log, TEXT("Alredy logged in: %s"), *NetId->ToString());
		return true;
	}*/

	bool LoginSuccess = false;

	FOnlineAccountCredentials Credentials;
	Credentials.Id = ID;
	Credentials.Token = Token;
	Credentials.Type = AuthType;

	Identity->OnLoginCompleteDelegates->AddUObject(this, &USG_GameInstance::HandleLoginCompleted);

	LoginSuccess = Identity->Login(0, Credentials);


	UE_LOG(LogTemp, Log, TEXT("Logging into EOS..."));

	if (!LoginSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to initiate login... "));
	}
	return LoginSuccess;
}

void USG_GameInstance::HandleLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Login callback completed!"));;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EOS login failed Error: %s"), *Error);
	}
}



FString USG_GameInstance::GenerateSessionCode()
{
	//Some generation
	return FString("Some code");
}

FString USG_GameInstance::CreateLobby(FName KeyName)
{

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	FString KeyValue = GenerateSessionCode();
	CreateLobbyDelegateHandle =
		Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleCreateLobbyCompleted));

	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->NumPublicConnections = 2;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	SessionSettings->bAllowInvites = false;
	SessionSettings->bAllowJoinInProgress = false;
	SessionSettings->bIsDedicated = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = true;
	SessionSettings->bUsesStats = true;
	SessionSettings->bIsLANMatch = false;
	SessionSettings->Settings.Add(KeyName, FOnlineSessionSetting((KeyValue), EOnlineDataAdvertisementType::ViaOnlineService));

	UE_LOG(LogTemp, Log, TEXT("Creating Lobby..."));

	LobbyName = KeyName;

	FUniqueNetIdRepl NetId = LocalPlayer->GetPreferredUniqueNetId();

	if (NetId.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("NetId.IsValid() == false"));
	}

	UE_LOG(LogTemp, Log, TEXT("Create lobby - net id: %s"), *NetId.GetUniqueNetId().Get()->ToString());

	if (!Session->CreateSession(*NetId, LobbyName, *SessionSettings))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create Lobby!"));
		return FString();
	}

	StartLobbyDelegateHandle =
		Session->AddOnStartSessionCompleteDelegate_Handle(FOnStartSessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleStartLobbyCompleted));

	return KeyValue;
}

void USG_GameInstance::HandleCreateLobbyCompleted(FName EOSLobbyName, bool bWasSuccessful)
{

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::HandleCreateLobbyCompleted IsValid(World) == false"));
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Lobby: %s Created!"), *EOSLobbyName.ToString());
		SetupNotifications();


		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		bool bIsInSession = Session->IsPlayerInSession(EOSLobbyName, *LocalPlayer->GetPreferredUniqueNetId());

		UE_LOG(LogTemp, Log, TEXT("Is host in session %d"), bIsInSession);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameOnly());
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientTravel(GameMap, TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create lobby!"));
	}

	Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateLobbyDelegateHandle);
	CreateLobbyDelegateHandle.Reset();
}

void USG_GameInstance::SetupNotifications()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	Session->AddOnSessionParticipantsChangeDelegate_Handle(FOnSessionParticipantsChangeDelegate::CreateUObject(
		this,
		&ThisClass::HandleParticipantChanged));
}

void USG_GameInstance::HandleParticipantChanged(FName EOSLobbyName, const FUniqueNetId& NetId, bool bJoined)
{
	if (bJoined)
	{
		UE_LOG(LogTemp, Log, TEXT("A player has joined Lobby: %s"), *LobbyName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("A player has left Lobby: %s"), *LobbyName.ToString());
	}
}

bool USG_GameInstance::FindLobbies(FName SearchKey, FString SearchValue)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	FUniqueNetIdRepl NetId = LocalPlayer->GetPreferredUniqueNetId();

	if (NetId.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("NetId.IsValid() == false"));
	}

	UE_LOG(LogTemp, Log, TEXT("Find lobby - net id: %s"), *NetId.GetUniqueNetId().Get()->ToString());

	UE_LOG(LogTemp, Warning, TEXT("session num: %d"), Session->GetNumSessions());
	FNamedOnlineSession* NamedSession = Session->GetNamedSession(SearchKey);
	if (NamedSession != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("session num: %s"), *NamedSession->SessionName.ToString());
	}

	// Remove the default search parameters that FOnlineSessionSearch sets up.
	Search->QuerySettings.SearchParams.Empty();
	Search->MaxSearchResults = 5;
	Search->QuerySettings.Set(SearchKey, SearchValue, EOnlineComparisonOp::Equals);
	Search->QuerySettings.Set(FName(TEXT("LOBBYSEARCH")), true, EOnlineComparisonOp::Equals);


	//Search->QuerySettings.Set("LOBBIESSEARCH", true, EOnlineComparisonOp::Equals);
	FindLobbiesDelegateHandle =
		Session->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleFindLobbiesCompleted,
			Search));
	UE_LOG(LogTemp, Log, TEXT("Finding lobby."));

	if (!Session->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), Search))
	{
		UE_LOG(LogTemp, Log, TEXT("Finding lobby failed."));
		// Clear our handle and reset the delegate. 
		Session->ClearOnFindSessionsCompleteDelegate_Handle(FindLobbiesDelegateHandle);
		FindLobbiesDelegateHandle.Reset();
		return false;
	}
	return true;
}

void USG_GameInstance::StartGame()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	HostingMode = "";
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::StartGame IsValid(World) == false"));
		return;
	}
	Session->StartSession(LobbyName);
}

void USG_GameInstance::ToggleVoiceChat(bool State)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->NumPublicConnections = 2;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	SessionSettings->bAllowInvites = false;
	SessionSettings->bAllowJoinInProgress = false;
	SessionSettings->bIsDedicated = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = State;
	SessionSettings->bUsesStats = true;
	SessionSettings->bIsLANMatch = false;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	FUniqueNetIdRepl NetId = LocalPlayer->GetPreferredUniqueNetId();


	State ? UGameplayStatics::GetPlayerController(GetWorld(), 0)->GameplayUnmutePlayer(NetId) : UGameplayStatics::GetPlayerController(GetWorld(), 0)->GameplayMutePlayer(NetId);

	Session->UpdateSession(LobbyName, *SessionSettings, true);

}

void USG_GameInstance::HandleStartLobbyCompleted(FName EOSLobbyName, bool bWasSuccessful)
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameOnly());
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientTravel(GameMap, TRAVEL_Absolute);
}

void USG_GameInstance::HandleFindLobbiesCompleted(bool bWasSuccessful, TSharedRef<FOnlineSessionSearch> Search)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (bWasSuccessful)
	{
		// added code here to not run into issues when searching for sessions is succesfull, but the number of sessions is 0
		if (Search->SearchResults.Num() == 0)
		{
			// If we're in P2P mode and we can't find a lobby on startup, create one. 
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Found lobby."));
		for (auto SessionInSearchResult : Search->SearchResults)
		{
			if (SessionInSearchResult.IsValid() == false)
			{
				//continue;

			}

			//Ensure the connection string is resolvable and store the info in ConnectString and in SessionToJoin
			if (Session->GetResolvedConnectString(SessionInSearchResult, NAME_GamePort, ConnectString))
			{
				LobbyToJoin = &SessionInSearchResult;
			}

			// For this course we will join the first session found automatically. Usually you would loop through all the sessions and determine which one is best to join. 
			break;
		}
		JoinLobby();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Find lobby failed."));
	}

	// Clear our handle and reset the delegate. 
	Session->ClearOnFindSessionsCompleteDelegate_Handle(FindLobbiesDelegateHandle);
	FindLobbiesDelegateHandle.Reset();
}



void USG_GameInstance::JoinLobby()
{
	// Tutorial 4: Join the session.
	// Tutorial 7: Same code is used to join the lobby - just some tweaks to the logging 

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	FUniqueNetIdRepl NetId = LocalPlayer->GetPreferredUniqueNetId();

	if (NetId.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("NetId.IsValid() == false"));
	}

	UE_LOG(LogTemp, Log, TEXT("Join lobby - net id: %s"), *NetId.GetUniqueNetId().Get()->ToString());

	JoinLobbyDelegateHandle =
		Session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleJoinLobbyCompleted));

	UE_LOG(LogTemp, Log, TEXT("Joining Lobby."));
	if (!Session->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), "SessionName", *LobbyToJoin))
	{
		UE_LOG(LogTemp, Log, TEXT("Join Lobby failed."));

		// Clear our handle and reset the delegate. 
		Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinLobbyDelegateHandle);
		JoinLobbyDelegateHandle.Reset();
	}



}

void USG_GameInstance::HandleJoinLobbyCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	StartLobbyDelegateHandle =
		Session->AddOnStartSessionCompleteDelegate_Handle(FOnStartSessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleStartLobbyCompleted));

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("Didnt join lobby"));
		Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinLobbyDelegateHandle);
		JoinLobbyDelegateHandle.Reset();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Joined lobby."));
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameOnly());
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientTravel(ConnectString, TRAVEL_Absolute);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientTravel(GameMap, TRAVEL_Absolute);
	// Clear our handle and reset the delegate. 
	Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinLobbyDelegateHandle);
	JoinLobbyDelegateHandle.Reset();
}