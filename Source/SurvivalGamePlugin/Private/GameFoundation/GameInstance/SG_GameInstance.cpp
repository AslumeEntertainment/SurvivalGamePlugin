// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/SG_GameInstance.h"

#include "Kismet/GameplayStatics.h" 
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"
#include "GameFoundation/GameInstance/GlobalWidgetSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(Networking, Log, All);

void USG_GameInstance::Init()
{
	Super::Init();


}

void USG_GameInstance::OnStart()
{
	Super::OnStart();

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &USG_GameInstance::HandleNetworkFailure);
	}

	USessionSubsystem* SessionSubsystemPtr = Cast<USessionSubsystem>(GetSubsystemBase(SessionSubsystemClass));
	if (IsValid(SessionSubsystemPtr) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::OnStart IsValid(SessionSubsystemPtr) == false - Subsystem is not of class BP_SessionSubsystem"));
		return;
	}

	UGlobalWidgetSubsystem* GlobalWidgetSubsystemPtr = Cast<UGlobalWidgetSubsystem>(GetSubsystemBase(GlobalWidgetSubsytemClass));
	if (IsValid(GlobalWidgetSubsystemPtr) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_GameInstance::OnStart IsValid(GlobalWidgetSubsystemPtr) == false - Subsystem is not of class BP_GlobalWidgetSubsystem"));
		return;
	}

	SessionSubsystemPtr->OnCoverUpWidgetNeeded.AddDynamic(GlobalWidgetSubsystemPtr, &UGlobalWidgetSubsystem::BeginLoadingScreen);
	SessionSubsystemPtr->OnCoverUpWidgetNotNeeded.AddDynamic(GlobalWidgetSubsystemPtr, &UGlobalWidgetSubsystem::EndLoadingScreen);
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