// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/IdentitySubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

DEFINE_LOG_CATEGORY_STATIC(IdentitySubsystem, Log, All);

void UIdentitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(IdentitySubsystem, Error, TEXT("Initialize IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(World);
	if (OnlineSubsystem == nullptr)
	{
		UE_LOG(IdentitySubsystem, Error, TEXT("Initialize IsValid(OnlineSubsystem) == false"));
		return;
	}

	IdentityInterface = OnlineSubsystem->GetIdentityInterface();

	if (IdentityInterface == nullptr)
	{
		UE_LOG(IdentitySubsystem, Error, TEXT("Initialize IsValid(IdentityInterface) == false"));
	}

	IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UIdentitySubsystem::HandleLoginComplete);
}

void UIdentitySubsystem::Deinitialize()
{
	IdentityInterface = nullptr;
}

void UIdentitySubsystem::HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{

	if (!bWasSuccessful)
	{
		UE_LOG(IdentitySubsystem, Warning, TEXT("EOS login failed Error: %s"), *Error);
		OnLoginError.Broadcast(FText::FromString("Login Failed: " + Error));
		return;

	}

	LocalPlayerNetId = IdentityInterface->GetUniquePlayerId(0);

	IdentityInterface->ClearOnLoginCompleteDelegates(0, this);
	OnLoginError.Broadcast(FText::FromString("Login Successful"));
	UE_LOG(IdentitySubsystem, Display, TEXT("Login callback completed!"));
	OnLoginComplete.Broadcast();
}

bool UIdentitySubsystem::Login()
{
	if (IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return true;
	}

	if (!IdentityInterface->AutoLogin(0 /* LocalUserNum */))
	{
		UE_LOG(IdentitySubsystem, Warning, TEXT("Could not start auto log in for local user..."));

		FOnlineAccountCredentials Credentials;
		Credentials.Id = "";
		Credentials.Token = "";
		Credentials.Type = "accountportal";

		if (!IdentityInterface->Login(0, Credentials))
		{
			UE_LOG(IdentitySubsystem, Error, TEXT("Could not start forced log in for local user..."));
			return false;
		}
	}
	UE_LOG(IdentitySubsystem, Display, TEXT("Started loggin in..."));
	return true;
}

FUniqueNetIdPtr UIdentitySubsystem::GetLocalPlayerNetId()
{
	return LocalPlayerNetId;
}

FString UIdentitySubsystem::GetLocalPlayerDisplayName()
{
	return IdentityInterface->GetPlayerNickname(*LocalPlayerNetId);
}

bool UIdentitySubsystem::IsPLayerLoggedIn()
{
	return LocalPlayerNetId != nullptr;
}
