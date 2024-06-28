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

