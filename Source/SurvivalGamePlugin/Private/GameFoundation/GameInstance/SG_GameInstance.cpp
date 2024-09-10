// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/SG_GameInstance.h"

#include "Kismet/GameplayStatics.h" 
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(Networking, Log, All);

void USG_GameInstance::OnStart()
{
	Super::OnStart();

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &USG_GameInstance::HandleNetworkFailure);
	}

}

void USG_GameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(Networking, Warning, TEXT("--------------------------------------------------NETWORK FAILURE-----------------------------------------"));
	UE_LOG(Networking, Error, TEXT("Error: %s") ,  *ErrorString);
	//fire of networking error delegate

	USessionSubsystem* SessionSubsystemPtr = GetSubsystem<USessionSubsystem>();
	if (IsValid(SessionSubsystemPtr) == false)
	{
		UE_LOG(Networking, Error, TEXT("CreateSession (IsValid(SessionSubsystemPtr) == false"));
		return;
	}

	SessionSubsystemPtr->DestroySession(true, true);
}