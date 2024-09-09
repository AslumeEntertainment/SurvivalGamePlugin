// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/UserCloudSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

#include "GameFoundation/GameInstance/IdentitySubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(UserCloudSubsystem, Log, All);

void UUserCloudSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("Initialize IsValid(World) == false"));
		return;
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
	if (Subsystem == nullptr)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("Initialize IsValid(Subsystem) == false"));
		return;
	}

	UserCloudInterface = Subsystem->GetUserCloudInterface();

	if (UserCloudInterface == nullptr)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("Initialize IsValid(UserCloudInterface) == false"));
		return;
	}

	UserCloudInterface->OnReadUserFileCompleteDelegates.AddUObject(this, &UUserCloudSubsystem::HandleLoadSaveFilecomplete);
	UserCloudInterface->OnWriteUserFileCompleteDelegates.AddUObject(this, &UUserCloudSubsystem::HandleWriteSaveFileComplete);
}

void UUserCloudSubsystem::Deinitialize()
{
	UserCloudInterface->ClearOnReadUserFileCompleteDelegates(this);
	UserCloudInterface->ClearOnWriteUserFileCompleteDelegates(this);
	UserCloudInterface = nullptr;
}

bool UUserCloudSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (GetClass()->IsInBlueprint() && Super::ShouldCreateSubsystem(Outer))
	{
		return true;
	}
	return false;
}

void UUserCloudSubsystem::HandleLoadSaveFilecomplete(bool bWasSuccessful, const FUniqueNetId& UserId, const FString& FileName)
{
	if (!bWasSuccessful)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("HandleLoadSaceFilecomplete Error finding file: %s"), *SaveFileName);
		return;
	}
	
	TArray<uint8> FileContents;
	if (!UserCloudInterface->GetFileContents(UserId, FileName, FileContents))
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("HandleLoadSaceFilecomplete Error reading file: %s"), *SaveFileName);
		return;
	}
	

	// The file data is in FileContents. If you wanted to read the file contents
	// as a save game, you could so like this:
	//USaveGame* Data = UGameplayStatics::LoadGameFromMemory(FileContents);
	
	
}

void UUserCloudSubsystem::HandleWriteSaveFileComplete(bool bWasSuccessful, const FUniqueNetId&, const FString& FileName)
{
	if (!bWasSuccessful)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("HandleWriteSaveFileComplete Error writing in file: %s"), *SaveFileName);
		return;
	}

	UE_LOG(UserCloudSubsystem, Display, TEXT("HandleWriteSaveFileComplete Writing in file: %s - Successful"), *SaveFileName);
}

void UUserCloudSubsystem::ReadSaveFile()
{
	UIdentitySubsystem* IdentitySubsystemPtr = GetGameInstance()->GetSubsystem<UIdentitySubsystem>();
	if (IsValid(IdentitySubsystemPtr) == false)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("LoadSaveFile (IsValid(IdentitySubsystemPtr) == false"));
		return;
	}

	bool bSuccess = UserCloudInterface->ReadUserFile(*IdentitySubsystemPtr->GetLocalPlayerNetId(), SaveFileName);

	if (!bSuccess)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("LoadSaveFile Could not start searching for file: %s"), *SaveFileName);
		return;
	}

	UE_LOG(UserCloudSubsystem, Display, TEXT("LoadSaveFile Searching for file: %s..."), *SaveFileName);
}

void UUserCloudSubsystem::WriteSaveFile()
{

	UIdentitySubsystem* IdentitySubsystemPtr = GetGameInstance()->GetSubsystem<UIdentitySubsystem>();
	if (IsValid(IdentitySubsystemPtr) == false)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("LoadSaveFile (IsValid(IdentitySubsystemPtr) == false"));
		return;
	}


	TArray<uint8> FileData;

	// If you wanted to write a Save Game to Player Data Storage, you could
	// populate FileData from a USaveGame* instance:
	//UGameplayStatics::SaveGameToMemory(SaveGame, FileData);

	UserCloudInterface->WriteUserFile(*IdentitySubsystemPtr->GetLocalPlayerNetId(), SaveFileName, FileData);
}




