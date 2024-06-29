// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/SG_GameInstance.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"

bool USG_GameInstance::Login(FString ID, FString Token, FString AuthType)
{

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::Login IsValid(World) == false"));
		return false;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::Login IsValid(Subsystem) == false"));
		return false;
	}

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (Identity == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::Login IsValid(Identity) == false"));
		return false;
	}

	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return true;
	}

	LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(0,FOnLoginComplete::FDelegate::CreateUObject(this,&USG_GameInstance::HandleLoginComplete));

	if (!Identity->AutoLogin(0 /* LocalUserNum */))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not start auto log in for local user..."));

		FOnlineAccountCredentials Credentials;
		Credentials.Id = ID;
		Credentials.Token = Token;
		Credentials.Type = AuthType;

		if (!Identity->Login(0, Credentials))
		{
			UE_LOG(LogTemp, Error, TEXT("Could not start auto log in for local user..."));
			return false;
		}
	}
	return true;
}

void USG_GameInstance::HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::HandleLoginComplete IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::HandleLoginComplete IsValid(Subsystem) == false"));
		return;
	}

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (Identity == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::HandleLoginComplete IsValid(Identity) == false"));
		return;
	}
	Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, this->LoginDelegateHandle);

	LoginDelegateHandle.Reset();

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Login callback completed!"));;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EOS login failed Error: %s"), *Error);
	}

}
/*
bool USG_GameInstance::CreateParty()
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::CreateParty IsValid(World) == false"));
		return false;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::CreateParty IsValid(Subsystem) == false"));
		return false;
	}

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (Identity == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::CreateParty IsValid(Identity) == false"));
		return false;
	}
	if (Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("USG_GameInstance::CreateParty Player is Not logged in"));
		return false;
	}

	IOnlinePartyPtr Party = Subsystem->GetPartyInterface();
	if (Party == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::CreateParty IsValid(Party) == false"));
		return false;
	}

	const FUniqueNetId& LocalUserId = *Identity->GetUniquePlayerId(0).Get();
	const FOnlinePartyTypeId PartyTypeId = IOnlinePartySystem::GetPrimaryPartyTypeId();
	FPartyConfiguration Config;
	Config.bIsAcceptingMembers = true;
	Config.MaxMembers = 4;

	bool bSuccess = Party->CreateParty(LocalUserId, PartyTypeId, Config, FOnCreatePartyComplete::CreateUObject(this, &USG_GameInstance::HandleLobbyCreated));

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to initiate party... "));
		return false;
	}
	
	return true;
}

void USG_GameInstance::HandleLobbyCreated(const FUniqueNetId& LocalUserId, const TSharedPtr<const FOnlinePartyId>& PartyId, const ECreatePartyCompletionResult Result)
{
	if (Result == ECreatePartyCompletionResult::Succeeded)
	{
		UE_LOG(LogTemp, Log, TEXT("Party Created"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Party NOT Created"));
}

bool USG_GameInstance::IsPlayerLoggedIn()
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::IsPlayerLoggedIn IsValid(World) == false"));
		return false;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (Identity == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::IsPlayerLoggedIn IsValid(Identity) == false"));
		return false;
	}

	if (Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
	{
		return false;
	}
	
	return true;
}

FString USG_GameInstance::GetLocalPlayerUsername()
{
	FString PlayerName = "ErrorName";
	
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::GetPlayerUsername IsValid(World) == false"));
		return PlayerName;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	if (Identity == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::GetPlayerUsername IsValid(Identity) == false"));
		return PlayerName;
	}

	if (Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("USG_GameInstance::GetPlayerUsername Player is Not logged in"));
		return PlayerName;
	}

	PlayerName = Identity->GetPlayerNickname(0);
	
	return PlayerName;
}
*/
