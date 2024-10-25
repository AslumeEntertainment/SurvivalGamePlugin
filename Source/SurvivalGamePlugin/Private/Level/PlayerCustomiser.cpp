// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/PlayerCustomiser.h"

#include "Kismet/GameplayStatics.h" 
#include "Net/UnrealNetwork.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "Characters/InGameCharacter.h"
#include "UI/HUD/InGameHUD.h"
#include "UI/Widgets/PlayerCustomisingWidget.h"



// Sets default values
APlayerCustomiser::APlayerCustomiser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


void APlayerCustomiser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(APlayerCustomiser, bIsActive, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(APlayerCustomiser, bIsTaken, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(APlayerCustomiser, bIsPlayerReady, COND_None, REPNOTIFY_Always);

}


void APlayerCustomiser::BeginPlay()
{

	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::BeginPlay IsValid(World) == false"));
		return;
	}

	//RIGHT NOW THAT MAKES ALL PLAYR CUSTOMISR CRAT A WIDGT IN BEGIN PLAY. THAT MEANS THE ARE LIKE 25 WIDGHTS AT ALL TIME
	//WHICH IS VERY CRINGW AND W MAY NED TO MOVE THAT CREATION IN INTERACT

	PlayerCustomisingWidget = CreateWidget<UPlayerCustomisingWidget>(World, PlayerCustomisingWidgetClass);

	if (IsValid(PlayerCustomisingWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::BeginPlay IsValid(PlayerCustomisingWidget) == false"));
		return;
	}

	PlayerCustomisingWidget->OnLeaveButton.AddDynamic(this, &APlayerCustomiser::Client_StopInteracting);
	PlayerCustomisingWidget->OnReadyButton.AddDynamic(this, &APlayerCustomiser::ToggleReady);
}

void APlayerCustomiser::InteractRequest(AController* InteractingCharacter)
{
	UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest - %s"), *InteractingCharacter->GetName());
	ASG_PlayerController* PC = Cast<ASG_PlayerController>(InteractingCharacter);

	if (IsValid(PC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest IsValid(InteractingPlayer) == false"));
		return;
	}

	SetOwner(InteractingCharacter);
	InteractingPlayer = PC;

	if (PC->IsLocalPlayerController() == true)
	{
		PC->Client_SetInputMode(EInputMode::UIOnly);
		PC->bShowMouseCursor = true;
		Client_ShowUI();
	}

	if (HasAuthority() == true)
	{
		OnPlayerStartInteracting.Broadcast(this);
		MainPawn = PC->GetPawn();

		//this makes it possible to tell the server you are changing states aka to use rpcs

		bIsTaken = true;
	}
}

bool APlayerCustomiser::bCanInteract(AController* InteractingCharacter)
{
	return bIsActive && !bIsTaken;
}

void APlayerCustomiser::Client_StopInteracting_Implementation()
{
	if (IsValid(InteractingPlayer) == true)
	{
		InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
		InteractingPlayer->bShowMouseCursor = false;
		InteractingPlayer = nullptr;
	}
	Client_HideUI();
	Server_StopInteracting();
}

void APlayerCustomiser::Server_StopInteracting_Implementation()
{
	OnPlayerStopInteracting.Broadcast(this);
	MainPawn = nullptr;
	GamePawn = nullptr;

	if (IsValid(InteractingPlayer) == true)
	{
		InteractingPlayer = nullptr;
	}

	bIsTaken = false;
}

void APlayerCustomiser::ToggleReady()
{
	PlayerCustomisingWidget->Config(EPlayerReadinessUIStatus::UpdatingStatus);

	Server_ToggleReady();
}

void APlayerCustomiser::Server_ToggleReady_Implementation()
{
	OnPlayerReadinessChanged.Broadcast(this);
}

void APlayerCustomiser::OnRepIsPlayerReady()
{
	PlayerCustomisingWidget->Config(bIsPlayerReady ? EPlayerReadinessUIStatus::Ready : EPlayerReadinessUIStatus::NotReady);
}

void APlayerCustomiser::UpdatePlayerReadiness(bool State)
{
	bIsPlayerReady = State;

	if (IsValid(PlayerCustomisingWidget) == false) return;

	PlayerCustomisingWidget->Config(bIsPlayerReady ? EPlayerReadinessUIStatus::Ready : EPlayerReadinessUIStatus::NotReady);
}

void APlayerCustomiser::SetIsActive_Server(bool State)
{
	bIsActive = State;
}

void APlayerCustomiser::SpawnHero(FVector Location, FRotator Rotation)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Server_SpawnHero_Implementation IsValid(World) == false"));
		return;
	}

	FActorSpawnParameters SpawnInfo;
	
	GamePawn = World->SpawnActor<AInGameCharacter>(DefaultHeroClass, Location, Rotation, SpawnInfo);
	if (IsValid(GamePawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Server_SpawnHero_Implementation IsValid(GamePawn) == false"));
		return;
	}

	GamePawn->OnCharacterDeath.AddDynamic(this, &APlayerCustomiser::InGameCharacterDead);
}

void APlayerCustomiser::InGameCharacterDead()
{
	OnInGameCharacterDeath.Broadcast(this);
}

void APlayerCustomiser::DespawnHero()
{
	if (IsValid(GamePawn) == false)
	{
		return;
	}
	GamePawn->Destroy();
	GamePawn = nullptr;
}

void APlayerCustomiser::PossessMainPawn()
{
	//InteractingPlayer->UnPossess();
	InteractingPlayer->Possess(MainPawn);
	InteractingPlayer->Client_SetInputMode(EInputMode::UIOnly);
	Client_ShowUI();
	Client_SetInGameOverlayVisibility(false);
	DespawnHero();
}

void APlayerCustomiser::PossessGamePawn()
{
	//InteractingPlayer->UnPossess();
	InteractingPlayer->Possess(GamePawn);
	InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
	Client_HideUI();
	Client_SetInGameOverlayVisibility(true);
}

void APlayerCustomiser::Client_SetInGameOverlayVisibility_Implementation(bool bShouldAddToViewport)
{
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Client_SetInGameOverlayVisibility_Implementation IsValid(InteractingPlayer) == false"));
		return;
	}
	AInGameHUD* HUD = InteractingPlayer->GetHUD<AInGameHUD>();
	if (IsValid(HUD) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Client_SetInGameOverlayVisibility_Implementation IsValid(HUD) == false"));
		return;
	}

	bShouldAddToViewport ? HUD->ShowGameplayOverlayWidget() : HUD->RemoveGameplayOverlayWidget();
}

void APlayerCustomiser::Client_HideUI_Implementation()
{
	PlayerCustomisingWidget->RemoveFromParent();
}

void APlayerCustomiser::Client_ShowUI_Implementation()
{
	PlayerCustomisingWidget->AddToViewport();
}


