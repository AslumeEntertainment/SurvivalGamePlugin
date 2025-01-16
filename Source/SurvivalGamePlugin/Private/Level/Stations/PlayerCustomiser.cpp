// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/Stations/PlayerCustomiser.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h" 
#include "Net/UnrealNetwork.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "FunctionLibraries/DeckFunctionLibrary.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "Characters/InGameCharacter.h"
#include "UI/Widgets/Lobby/PlayerCustomisingWidget.h"
#include "UI/Widgets/Deck/CardSelectorWidget.h"
#include "UI/Widgets/Core/UnitIconWidget.h"
#include "UI/HUD/InGameHUD.h"
#include "Level/GameManager.h"


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

bool APlayerCustomiser::InteractRequest(AController* InteractingCharacter)
{

	//this makes it possible to tell the server you are changing states aka to use RPCs
	SetOwner(InteractingCharacter);

	UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest - %s"), *InteractingCharacter->GetName());
	if(Super::InteractRequest(InteractingCharacter)==true)
	{
		ShowUI();
	}

	if (HasAuthority() == true)
	{
		OnPlayerStartInteracting.Broadcast(this);
		MainPawn = Cast<ASG_Character>(InteractingPlayer->GetPawn());

		InteractingPlayer->OnConnectionCleanUp.AddDynamic(this, &APlayerCustomiser::HandlePlayerLeavingSession);

		bIsTaken = true;
	}

	return true;
}

bool APlayerCustomiser::bCanInteract(AController* InteractingCharacter)
{
	return bIsActive && !bIsTaken && !PlayerCustomisingWidget->IsInViewport();
}

void APlayerCustomiser::Client_StopInteracting_Implementation()
{
	if (IsValid(InteractingPlayer) == true)
	{
		StopInteracting();
	}
}

void APlayerCustomiser::StopInteracting()
{
	HideUI(true);

	InteractingPlayer->OnConnectionCleanUp.RemoveAll(this);
	InteractingPlayer->SetViewTargetWithBlend(InteractingPlayer->GetPawn(), 0.25f, EViewTargetBlendFunction::VTBlend_Linear);
	Super::StopInteracting();
	
	Server_StopInteracting();
}

void APlayerCustomiser::Server_StopInteracting_Implementation()
{
	OnPlayerStopInteracting.Broadcast(this);
	MainPawn = nullptr;
	GamePawn = nullptr;

	if (IsValid(InteractingPlayer) == true)
	{
		InteractingPlayer->OnConnectionCleanUp.RemoveAll(this);
		InteractingPlayer = nullptr;
	}

	bIsTaken = false;
}

void APlayerCustomiser::ToggleReady()
{
	PlayerCustomisingWidget->Config(EPlayerReadinessUIStatus::UpdatingStatus);

	Server_ToggleReady();
}

void APlayerCustomiser::OpenCardSelector(UPanelWidget* CallerWidget)
{
	if (!IsValid(HeroTable) || !IsValid(MainHandResourceTable) || !IsValid(SecondaryHandResourceTable))
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::OpenCardSelector One or more card data tables are invalid"));
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::OpenCardSelector IsValid(World) == false"));
		return;
	}

	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::OpenCardSelector IsValid(InteractingPlayer) == false"));
		return;
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::OpenCardSelector IsValid(Save) == false"));
		return;
	}

	TArray<FText> CardNames;
	TArray<UTexture2D*> CardIcons;
	TArray<FLinearColor> CardColors;

	FString Title = "";
	int CardsNeeded = 0;

	if (CallerWidget == PlayerCustomisingWidget->GetHeroCardPanel())
	{
		TArray<FHeroCardPersistentData*> HeroDataRows = UDeckFunctionLibrary::GetPlayerHeroSaveData(Save, HeroTable);
		for (FHeroCardPersistentData* HeroRow : HeroDataRows)
		{
			CardNames.Add(HeroRow->UnitName);
			CardIcons.Add(HeroRow->UnitImage);
			CardColors.Add(HeroRow->UnitColor);
		}
	
		CardsNeeded = 1;
		Title = "Select a hero card";
	}
	if (CallerWidget == PlayerCustomisingWidget->GetMainCardsPanel())
	{
		TArray<FResourseCardPersistentData*> MainResourceDataRows;
		UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, MainHandResourceTable, TArray<uint16>()).GenerateValueArray(MainResourceDataRows);
		for (FResourseCardPersistentData* ResourceRow : MainResourceDataRows)
		{
			CardNames.Add(ResourceRow->UnitName);
			CardIcons.Add(ResourceRow->UnitImage);
			CardColors.Add(ResourceRow->UnitColor);
		}

		CardsNeeded = 3;
		Title = "Select main hand resource cards";
	}
	if (CallerWidget == PlayerCustomisingWidget->GetSecondaryCardsPanel())
	{
		TArray<FResourseCardPersistentData*> SecondaryResourceDataRows;
		UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, SecondaryHandResourceTable, TArray<uint16>()).GenerateValueArray(SecondaryResourceDataRows);
		for (FResourseCardPersistentData* ResourceRow : SecondaryResourceDataRows)
		{
			CardNames.Add(ResourceRow->UnitName);
			CardIcons.Add(ResourceRow->UnitImage);
			CardColors.Add(ResourceRow->UnitColor);
		}
		
		CardsNeeded = 3;
		Title = "Select secondary hand resource cards";
	}

	UCardSelectorWidget* CardSelector = CreateWidget<UCardSelectorWidget>(World, CardSelectorWidgetClass);
	if (IsValid(CardSelector))
	{
		CardSelector->ConfigCardSelector(CallerWidget, CardsNeeded);
		CardSelector->FillCards(CardNames, CardIcons, CardColors);
		CardSelector->SetTitle(Title);
		CardSelector->OnNeededCardsSelected.AddDynamic(this, &APlayerCustomiser::DisplayDeckCards);

		PlayerCustomisingWidget->OpenCardSelector(CardSelector);
	}
}

void APlayerCustomiser::DisplayDeckCards(uint8 NumCardsToDisplay, TArray<uint16> CardIndexes, UPanelWidget* CardBox)
{
	if (!IsValid(HeroTable) || !IsValid(MainHandResourceTable) || !IsValid(SecondaryHandResourceTable))
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::DisplayDeckCards One or more card data tables are invalid"));
		return;
	}

	if (IsValid(PlayerCustomisingWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::DisplayDeckCards IsValid(PlayerCustomisingWidget) == false"));
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::DisplayDeckCards IsValid(World) == false"));
		return;
	}

	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::DisplayDeckCards IsValid(InteractingPlayer) == false"));
		return;
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::DisplayDeckCards IsValid(Save) == false"));
		return;
	}

	UUnitIconWidget* TempCard = nullptr;

	if (CardBox == PlayerCustomisingWidget->GetHeroCardPanel())
	{
		TArray<FHeroCardPersistentData*> HeroDataRows = UDeckFunctionLibrary::GetPlayerHeroSaveData(Save, HeroTable);
		for (int i : CardIndexes)
		{
			if (i >= HeroDataRows.Num() || HeroDataRows[i] == nullptr)
			{
				continue;
			}
			TempCard = PlayerCustomisingWidget->FillHeroCardSlot(HeroDataRows[i]->UnitName,
				HeroDataRows[i]->UnitImage, HeroDataRows[i]->HeroBackgroundImage, HeroDataRows[i]->UnitColor);
			if (IsValid(TempCard))
			{
				TempCard->BroadcastParentOnClicked.AddDynamic(this, &APlayerCustomiser::OpenCardSelector);
			}
		}
		return;
	}

	TMap<uint16, FResourseCardPersistentData*> SavedCards;

	if (CardBox == PlayerCustomisingWidget->GetMainCardsPanel())
	{
		SavedCards = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, MainHandResourceTable, TArray<uint16>());
	}
	else if (CardBox == PlayerCustomisingWidget->GetSecondaryCardsPanel())
	{
		SavedCards = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, SecondaryHandResourceTable, TArray<uint16>());
	}

	for (int i =0; i< NumCardsToDisplay; i++)
	{
		FResourseCardPersistentData** CardPtr;
		FResourseCardPersistentData Card;
		if (i < CardIndexes.Num())
		{
			CardPtr = SavedCards.Find(i);
			if (CardPtr == nullptr)
			{
				continue;
			}
			Card = **CardPtr;
		}
		else
		{
			Card = UDeckFunctionLibrary::GenerateCardWithID(SecondaryHandResourceTable, 0);
		}

		if (CardBox == PlayerCustomisingWidget->GetMainCardsPanel())
		{
			TempCard = PlayerCustomisingWidget->FillSingleMainHandCardSlot(Card.UnitName ,
				Card.UnitImage, Card.UnitColor);
		}
		else if (CardBox == PlayerCustomisingWidget->GetSecondaryCardsPanel())
		{
			TempCard = PlayerCustomisingWidget->FillSingleSecondaryHandCardSlot(Card.UnitName,
				Card.UnitImage, Card.UnitColor);
		}
		if (IsValid(TempCard))
		{
			TempCard->BroadcastParentOnClicked.AddDynamic(this, &APlayerCustomiser::OpenCardSelector);
		}
	}
}

void APlayerCustomiser::Server_ToggleReady_Implementation()
{
	OnPlayerReadinessChanged.Broadcast(this);
}

void APlayerCustomiser::OnRepIsPlayerReady()
{
	PlayerCustomisingWidget->Config(bIsPlayerReady ? EPlayerReadinessUIStatus::Ready : EPlayerReadinessUIStatus::NotReady);
}

void APlayerCustomiser::HandlePlayerLeavingSession(APlayerController* LeavingPlayer)
{
	Server_StopInteracting();
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
	GamePawn->Cliet_UnbindOverlayDelegates();
	InteractingPlayer->Possess(MainPawn);
	InteractingPlayer->Client_SetInputMode(EInputMode::UIOnly);
	Client_ShowUI();
	Client_SetInGameOverlayVisibility(false);
	DespawnHero();
}

void APlayerCustomiser::PossessGamePawn()
{
	MainPawn->Cliet_UnbindOverlayDelegates();
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

void APlayerCustomiser::Client_HideUI_Implementation(bool bUsesAnimation)
{
	HideUI(bUsesAnimation);
}

void APlayerCustomiser::HideUI(bool bUsesAnimation)
{
	AInGameHUD* HUD = Cast<AInGameHUD>(InteractingPlayer->GetHUD());
	if (IsValid(HUD))
	{
		HUD->ShowInteractionOverlayWidget();
		InteractingPlayer->bShowMouseCursor = false;
	}

	bUsesAnimation ? PlayerCustomisingWidget->CloseWidget() : PlayerCustomisingWidget->RemoveFromParent();
}

void APlayerCustomiser::ShowUI()
{
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::ShowUI IsValid(InteractingPlayer) == false"));
		return;
	}

	if (!IsValid(HeroTable) || !IsValid(MainHandResourceTable) || !IsValid(SecondaryHandResourceTable))
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::ShowUI One or more card data tables are invalid"));
		return;
	}

	if (IsValid(GetParentActor()))
	{
		InteractingPlayer->SetViewTargetWithBlend(GetParentActor(), 0.25f, EViewTargetBlendFunction::VTBlend_Linear);
		AInGameHUD* HUD = Cast<AInGameHUD>(InteractingPlayer->GetHUD());
		if (IsValid(HUD))
		{
			HUD->RemoveInteractionOverlayWidget();
			InteractingPlayer->bShowMouseCursor = true;
		}
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::ShowUI IsValid(Save) == false"));
		return;
	}

	FDeckPersistentData ActiveDeck = Save->Decks[Save->ActiveDeckIndex];

	TArray<uint16> NeededHeroCards;
	NeededHeroCards.Add(ActiveDeck.Hero);

	PlayerCustomisingWidget->ClearMainCardsPanel();
	PlayerCustomisingWidget->ClearSecondaryCardsPanel();

	DisplayDeckCards(NumHeroesInDeck, NeededHeroCards, PlayerCustomisingWidget->GetHeroCardPanel());
	DisplayDeckCards(NumEquipmentInDeck - NumEquipmentInDeck/2, ActiveDeck.MainEquipment, PlayerCustomisingWidget->GetMainCardsPanel());
	DisplayDeckCards(NumEquipmentInDeck / 2, ActiveDeck.SecondaryEquipment, PlayerCustomisingWidget->GetSecondaryCardsPanel());

	PlayerCustomisingWidget->AddToViewport();
}

void APlayerCustomiser::Client_ShowUI_Implementation()
{
	ShowUI();
}


