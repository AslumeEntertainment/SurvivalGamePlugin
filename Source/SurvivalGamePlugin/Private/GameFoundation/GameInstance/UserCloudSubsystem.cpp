// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/UserCloudSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

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

	UserCloudInterface->OnReadUserFileCompleteDelegates.AddUObject(this, &UUserCloudSubsystem::HandleReadSaveFileComplete);
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

void UUserCloudSubsystem::HandleReadSaveFileComplete(bool bWasSuccessful, const FUniqueNetId& UserId, const FString& FileName)
{

	if (!bWasSuccessful)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("HandleLoadSaveFileComplete Error finding file: %s"), *FileName);
		if (FileName == SaveFileName) OnGameRead.Broadcast(nullptr);
		return;
	}

	UE_LOG(UserCloudSubsystem, Display, TEXT("HandleReadSaveFileComplete Reading file: %s - Successful"), *FileName);

	if (FileName != SaveFileName)
	{
		return;
	}

	TArray<uint8> FileContents;
	if (!UserCloudInterface->GetFileContents(UserId, FileName, FileContents))
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("HandleLoadSaveFileComplete Error reading file: %s"), *FileName);
		OnError.Broadcast("Read Game Data Failed with code: -3(No such fail)");
		return;
	}
	
	USaveGame* Data = UGameplayStatics::LoadGameFromMemory(FileContents);
	
	OnGameRead.Broadcast(Data);
}

void UUserCloudSubsystem::HandleWriteSaveFileComplete(bool bWasSuccessful, const FUniqueNetId&, const FString& FileName)
{
	if (!bWasSuccessful)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("HandleWriteSaveFileComplete Error writing in file: %s"), *FileName);
		if (FileName == SaveFileName) OnError.Broadcast("Write Game Data Failed with code: -2");
		return;
	}

	if (FileName != SaveFileName)
	{
		return;
	}

	UE_LOG(UserCloudSubsystem, Display, TEXT("HandleWriteSaveFileComplete Writing in file: %s - Successful"), *FileName);
	OnWriteFinished.Broadcast();
}

void UUserCloudSubsystem::ReadSaveFile()
{
	UIdentitySubsystem* IdentitySubsystemPtr = GetGameInstance()->GetSubsystem<UIdentitySubsystem>();
	if (IsValid(IdentitySubsystemPtr) == false)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("ReadSaveFile (IsValid(IdentitySubsystemPtr) == false"));
		return;
	}

	bool bSuccess = UserCloudInterface->ReadUserFile(*IdentitySubsystemPtr->GetLocalPlayerNetId(), SaveFileName);

	if (!bSuccess)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("ReadSaveFile Could not start searching for file: %s"), *SaveFileName);
		OnError.Broadcast("Read Game Data Failed with code: -1");
		return;
	}

	UE_LOG(UserCloudSubsystem, Display, TEXT("LoadSaveFile Searching for file: %s..."), *SaveFileName);
}

void UUserCloudSubsystem::WriteSaveFile(USaveGame* GameToSave)
{

	UIdentitySubsystem* IdentitySubsystemPtr = GetGameInstance()->GetSubsystem<UIdentitySubsystem>();
	if (IsValid(IdentitySubsystemPtr) == false)
	{
		UE_LOG(UserCloudSubsystem, Error, TEXT("WriteSaveFile (IsValid(IdentitySubsystemPtr) == false"));
		OnError.Broadcast("Write Game Data Failed with code: -1");
		return;
	}


	TArray<uint8> FileData;

	// If you wanted to write a Save Game to Player Data Storage, you could
	// populate FileData from a USaveGame* instance:
	UGameplayStatics::SaveGameToMemory(GameToSave, FileData);

	FUniqueNetIdPtr LocalNetId = IdentitySubsystemPtr->GetLocalPlayerNetId();
	if (LocalNetId == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UUserCloudSubsystem::WriteSaveFile NetId == LocalNetId"));
		return;
	}

	UserCloudInterface->WriteUserFile(*LocalNetId, SaveFileName, FileData);
}




