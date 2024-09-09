// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/FriendsSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(FriendsSubsystem, Log, All);

void UFriendsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(FriendsSubsystem, Error, TEXT("Initialize IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(FriendsSubsystem, Error, TEXT("Initialize IsValid(Subsystem) == false"));
		return;
	}

	FriendsInterface = Subsystem->GetFriendsInterface();

	if (FriendsInterface == nullptr)
	{
		UE_LOG(FriendsSubsystem, Error, TEXT("Initialize IsValid(FriendsInterface) == false"));
		return;
	}
}

void UFriendsSubsystem::Deinitialize()
{
	FriendList.Empty();
	FriendsInterface = nullptr;
}

void UFriendsSubsystem::HandleReadFriendListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	if (!bWasSuccessful)
	{
		//fire off error delegate for this shit
		UE_LOG(FriendsSubsystem, Error, TEXT("Failed to read friends list..."));
		return;
	}

	UE_LOG(FriendsSubsystem, Display, TEXT("Read friends list successfully"));

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

void UFriendsSubsystem::ReadFriendList()
{
	bool bSuccess = FriendsInterface->ReadFriendsList(
		0 /* LocalUserNum */,
		TEXT("") /* ListName, unused by EOS */,
		FOnReadFriendsListComplete::CreateUObject(this, &UFriendsSubsystem::HandleReadFriendListComplete)
	);

	if (!bSuccess)
	{
		//fire off error delegate for this shit
		UE_LOG(FriendsSubsystem, Error, TEXT("Failed to start reading friends list..."));
		return;
	}

	UE_LOG(FriendsSubsystem, Display, TEXT("Started reading friends list..."));
}

void UFriendsSubsystem::InviteFriendToSession(FString FriendName)
{
	FUniqueNetIdRef* FriendNetIdPtr = FriendList.Find(FriendName);
	if (FriendNetIdPtr == nullptr)
	{
		UE_LOG(FriendsSubsystem, Error, TEXT("InviteFriendToSession FriendNetIdPtr == nullptr No friend with name: %s"), *FriendName);
		return;
	}
	
	USessionSubsystem* SessionSubsystemPtr = GetGameInstance()->GetSubsystem<USessionSubsystem>();
	if (IsValid(SessionSubsystemPtr) == false)
	{
		UE_LOG(FriendsSubsystem, Error, TEXT("InviteFriendToSession (IsValid(SessionSubsystemPtr) == false"));
		return;
	}

	SessionSubsystemPtr->InviteFriendToSession(*FriendNetIdPtr);

}
