// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Interfaces/OnlineFriendsInterface.h"

#include "Headers/GeneralDelegates.h"

#include "FriendsSubsystem.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UFriendsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	IOnlineFriendsPtr FriendsInterface = nullptr;

	TMap<FString, FUniqueNetIdRef> FriendList;

	void Initialize(FSubsystemCollectionBase& Collection) override;

	void Deinitialize() override;

	void HandleReadFriendListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

public:

	UFUNCTION(BlueprintCallable)
	void ReadFriendList();

	UFUNCTION(BlueprintCallable)
	void InviteFriendToSession(FString FriendName);


	FStringArrayTransferSignature OnFreindListUpdated;
};
