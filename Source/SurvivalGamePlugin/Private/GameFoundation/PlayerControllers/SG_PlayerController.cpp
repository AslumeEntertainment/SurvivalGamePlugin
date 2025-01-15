// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

#include "Kismet/GameplayStatics.h" 
#include "Net/UnrealNetwork.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"
#include "GameFoundation/GameInstance/UserCloudSubsystem.h"
#include "GameFoundation/GameInstance/GlobalWidgetSubsystem.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "UI/HUD/InGameHUD.h"
#include "Components/EquipmentComponent.h"
#include "Characters/InGameCharacter.h"

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
	//UserCloudSubsystemPtr->OnWriteFinished.AddDynamic(this, &ASG_PlayerController::ReadSave);  -  commented so that all clinet side changes are preditions only and send to the cloud
	OnReadSaveRequested.AddDynamic(UserCloudSubsystemPtr, &UUserCloudSubsystem::ReadSaveFile);
	OnWriteSaveRequested.AddDynamic(UserCloudSubsystemPtr, &UUserCloudSubsystem::WriteSaveFile);

	ReadSave();
}

void ASG_PlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);

	ASG_Character* ControlledCharacter = Cast<ASG_Character>(InPawn);

	if (IsValid(ControlledCharacter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT ACKNOWEGE POSSESION "));
		return;
	}

	AInGameHUD* HUD = Cast<AInGameHUD>(GetHUD());
	if (IsValid(HUD) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("INVALID HUDDDDDDDDDDD "));
		return;
	}

	HUD->SetInteractionOverlayBindings(ControlledCharacter);
	
	AInGameCharacter* InGameCharacter = Cast<AInGameCharacter>(ControlledCharacter);
	if (IsValid(InGameCharacter) == false) return;

	InGameCharacter->OnCharacterDeath.AddDynamic(HUD, &AInGameHUD::ClearScreen);
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
		Cheat_ClearAccount();
		//Save = Cast<USG_SaveGame>(UGameplayStatics::CreateSaveGameObject(USG_SaveGame::StaticClass()));
		//Save->Init();
		//WriteSave();
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

void ASG_PlayerController::SpawnItemInFrontOfPlayer(bool bShouldEquip, TSubclassOf<AItemBase> ItemClass, FItemDataStruct ItemData)
{
	if (HasAuthority() == false)
	{
		Server_SpawnItemInFrontOfPlayer(bShouldEquip, ItemClass, ItemData);
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::SpawnItemInFrontOfPlayer IsValid(World) == false"));
		return;
	}
	AInGameCharacter* PlayerPawn = Cast<AInGameCharacter>(GetPawn());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::SpawnItemInFrontOfPlayer IsValid(Player) == false"));
		return;
	}
	UEquipmentComponent* EquipmentComp = PlayerPawn->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASG_PlayerController::SpawnItemInFrontOfPlayer IsValid(EquipmentComp) == false"));
		return;
	}

	AItemBase* NewItem = World->SpawnActor<AItemBase>(ItemClass, PlayerPawn->GetPlayerLookAtLocation(100.f), PlayerPawn->GetActorRotation());

	if (IsValid(NewItem) && bShouldEquip)
	{
		EquipmentComp->EquipItem(NewItem);
	}
}

void ASG_PlayerController::Server_SpawnItemInFrontOfPlayer_Implementation(bool bShouldEquip, TSubclassOf<AItemBase> ItemClass, FItemDataStruct ItemData)
{
	SpawnItemInFrontOfPlayer(bShouldEquip, ItemClass, ItemData);
}

USG_SaveGame* ASG_PlayerController::GetSave()
{
	return Save;
}

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
	Save->Init();
	WriteSave();
}

