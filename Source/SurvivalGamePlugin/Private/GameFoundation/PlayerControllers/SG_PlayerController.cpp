// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

#include "Kismet/GameplayStatics.h" 
#include "Net/UnrealNetwork.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"
#include "GameFoundation/GameInstance/UserCloudSubsystem.h"
#include "GameFoundation/GameInstance/GlobalWidgetSubsystem.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "GameFoundation/PlayerStates/SG_PlayerState.h"
#include "GAS/SG_AbilitySystemComponent.h"

void ASG_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ASG_PlayerController, ASC, COND_None, REPNOTIFY_OnChanged);
}

void ASG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() == false)
	{
		return;
	}

	UUserCloudSubsystem* UserCloudSubsystemPtr = GetGameInstance()->GetSubsystem<UUserCloudSubsystem>();
	if (IsValid(UserCloudSubsystemPtr) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::BeginPlay (IsValid(UserCloudSubsystemPtr) == false"));
		return;
	}

	UserCloudSubsystemPtr->OnGameRead.AddDynamic(this, &ASG_PlayerController::ReceiveReadSave);
	UserCloudSubsystemPtr->OnWriteFinished.AddDynamic(this, &ASG_PlayerController::ReadSave);
	OnReadSaveRequested.AddDynamic(UserCloudSubsystemPtr, &UUserCloudSubsystem::ReadSaveFile);
	OnWriteSaveRequested.AddDynamic(UserCloudSubsystemPtr, &UUserCloudSubsystem::WriteSaveFile);

	ReadSave();
}

void ASG_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (bIsCameraMovementConstrained)
	{
		PlayerCameraManager->ViewYawMin = -80.f;
		PlayerCameraManager->ViewYawMax = 80.f;
		PlayerCameraManager->ViewPitchMin = -55.f;
		PlayerCameraManager->ViewPitchMax = 55.f;
	}

}

void ASG_PlayerController::InitASC(AActor* AvatarActor)
{
	ASG_PlayerState* SG_PlayerState = GetPlayerState<ASG_PlayerState>();
	if (IsValid(SG_PlayerState) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::InitASC IsValid(SG_PlayerState) == false"));
		return;
	}
	ASC = Cast<USG_AbilitySystemComponent>(SG_PlayerState->GetAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::InitASC IsValid(ASC) == false"));
		return;
	}
	
	ASC->InitAbilityActorInfo(PlayerState, AvatarActor);
}

FActiveGameplayEffectHandle ASG_PlayerController::AddEffect(TSubclassOf<UGameplayEffect> GameplayEffect, uint8 Level)
{
	return ASC->AddEffectFromClass(GameplayEffect, Level);
}

void ASG_PlayerController::RemoveEffect(FActiveGameplayEffectHandle RemovedEffectSpecHandle, int8 StacksToRemove)
{
	ASC->RemoveEffectFromSpec(RemovedEffectSpecHandle, StacksToRemove);
}

FGameplayAbilitySpecHandle ASG_PlayerController::AddAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, uint8 Level)
{
	return ASC->AddAbilityFromClass(AbilityClass, Level);
}

void ASG_PlayerController::RemoveAbility(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle)
{
	ASC->RemoveAbilityFromSpec(RemovedAbilitySpecHandle);
}

void ASG_PlayerController::Client_SetInputMode_Implementation(EInputMode InputMode)
{
	switch (InputMode)
	{
		case EInputMode::GameOnly: SetInputMode(FInputModeGameOnly()); break;
		case EInputMode::GameAndUI : SetInputMode(FInputModeGameAndUI()); break;
		case EInputMode::UIOnly: SetInputMode(FInputModeUIOnly()); break;
	}
}

void ASG_PlayerController::Client_LeaveSession_Implementation(bool bWasKicked)
{
	USessionSubsystem* SessionSubsystemPtr = GetGameInstance()->GetSubsystem<USessionSubsystem>();
	if (IsValid(SessionSubsystemPtr) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::Client_LeaveSession_Implementation (IsValid(SessionSubsystem) == false"));
		return;
	}

	UGlobalWidgetSubsystem* GlobalWidgetSubsystemPtr = GetGameInstance()->GetSubsystem<UGlobalWidgetSubsystem>();
	if (IsValid(GlobalWidgetSubsystemPtr) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::Client_LeaveSession_Implementation (IsValid(GlobalWidgetSubsystemPtr) == false"));
		return;
	}
	GlobalWidgetSubsystemPtr->ShowCoverupWidget();
	SessionSubsystemPtr->DestroySession();

}

void ASG_PlayerController::ReceiveReadSave(USaveGame* SaveGame)
{
	UE_LOG(LogTemp, Display, TEXT("ASG_PlayerController::ReceiveReadSave Read save complete"));

	Save = Cast<USG_SaveGame>(SaveGame);
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("No save game found - creating new file..."));
		Save = Cast<USG_SaveGame>(UGameplayStatics::CreateSaveGameObject(USG_SaveGame::StaticClass()));
		WriteSave();
	}

	OnSaveUpdated.Broadcast();

}

void ASG_PlayerController::ReadSave()
{
	UE_LOG(LogTemp, Display, TEXT("ASG_PlayerController::ReadSave Requesting Read"));
	OnReadSaveRequested.Broadcast();
}

void ASG_PlayerController::WriteSave()
{
	UE_LOG(LogTemp, Display, TEXT("ASG_PlayerController::WriteSave Requesting Write"));
	OnWriteSaveRequested.Broadcast(Save);
}

USG_SaveGame* ASG_PlayerController::GetSave()
{
	return Save;
}


//void ASG_PlayerController::InitPlayerState()
//{
//	Super::InitPlayerState();
//
//	UE_LOG(LogTemp, Warning, TEXT("ASG_PlayerController::InitPlayerState"));
//
//}

//void ASG_PlayerController::OnRep_ASC()
//{
//	UE_LOG(LogTemp, Warning, TEXT("ASG_PlayerController::OnRep_ASC"));
//}

//void ASG_PlayerController::OnRep_PlayerState()
//{
//	Super::OnRep_PlayerState();
//
//	UE_LOG(LogTemp, Warning, TEXT("ASG_PlayerController::OnRep_PlayerState"));
//}

void ASG_PlayerController::OnNetCleanup(UNetConnection* Connection)
{
	if (GetLocalRole() == ROLE_Authority && PlayerState != NULL)
	{

		OnConnectionCleanUp.Broadcast(this);
	}

	Super::OnNetCleanup(Connection);
}

void ASG_PlayerController::Cheat_SetMoney(uint32 Money)
{
	Save->Money = Money;
	WriteSave();
}

void ASG_PlayerController::Cheat_ClearAccount()
{
	Save = Cast<USG_SaveGame>(UGameplayStatics::CreateSaveGameObject(USG_SaveGame::StaticClass()));
	WriteSave();
}

