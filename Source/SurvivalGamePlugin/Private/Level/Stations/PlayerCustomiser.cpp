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
#include "UI/Widgets/Deck/DeckSegmentPanel.h"
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
	PlayerCustomisingWidget->DeckSelected.AddDynamic(this, &APlayerCustomiser::SwitchDeck);
	OnDeckPlayable.AddDynamic(PlayerCustomisingWidget, &UPlayerCustomisingWidget::DeckPlayabilityChanged);


	UDeckSegmentPanel* HeroPanel = PlayerCustomisingWidget->GetHeroCardPanel();
	UDeckSegmentPanel* MainPanel = PlayerCustomisingWidget->GetMainCardsPanel();
	UDeckSegmentPanel* SecondaryPanel = PlayerCustomisingWidget->GetSecondaryCardsPanel();

	HeroPanel->Init(EDeckSegment::Hero);
	MainPanel->Init(EDeckSegment::MainEquipment);
	SecondaryPanel->Init(EDeckSegment::SecondaryEquipment);

	HeroPanel->OnCardClicked.AddDynamic(this, &APlayerCustomiser::OpenCardSelector);
	MainPanel->OnCardClicked.AddDynamic(this, &APlayerCustomiser::OpenCardSelector);
	SecondaryPanel->OnCardClicked.AddDynamic(this, &APlayerCustomiser::OpenCardSelector);

	SegmentPanels.Add(EDeckSegment::Hero, HeroPanel);
	SegmentPanels.Add(EDeckSegment::MainEquipment, MainPanel);
	SegmentPanels.Add(EDeckSegment::SecondaryEquipment, SecondaryPanel);
}

bool APlayerCustomiser::InteractRequest(AController* InteractingCharacter)
{

	//this makes it possible to tell the server you are changing states aka to use RPCs
	SetOwner(InteractingCharacter);

	UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest - %s"), *InteractingCharacter->GetName());
	if(Super::InteractRequest(InteractingCharacter)==true)
	{
		SendDeckData();
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

void APlayerCustomiser::OpenCardSelector(EDeckSegment Segment)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::OpenCardSelector IsValid(World) == false"));
		return;
	}

	if (!IsValid(ResourceTable))
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::OpenCardSelector ResourceTable data table is invalid"));
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
	TArray<uint16> CardIndexes;

	FSegementDefinition* DefinitionPtr = SegmentDefinitions.Find(Segment);

	if (DefinitionPtr == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::OpenCardSelector Segment %s is not propperly defined check SegmentDefinitions property"), *UEnum::GetValueAsString(Segment));
		return;
	}

	TMap<uint16, FResourseCardPersistentData*> Cards = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, ResourceTable, DefinitionPtr->Series);
	for (TPair<uint16, FResourseCardPersistentData*> ResourceRow : Cards)
	{
		CardIndexes.Add(ResourceRow.Key);
		CardNames.Add(ResourceRow.Value->UnitName);
		CardIcons.Add(ResourceRow.Value->UnitImage);
		CardColors.Add(ResourceRow.Value->UnitColor);
	}

	UCardSelectorWidget* CardSelector = CreateWidget<UCardSelectorWidget>(World, CardSelectorWidgetClass);
	if (IsValid(CardSelector))
	{
		TArray<uint16> AlredySelectedCards;
		switch (Segment)
		{
		case EDeckSegment::Hero: AlredySelectedCards = Save->Decks[Save->ActiveDeckIndex].Heroes;
			break;
		case EDeckSegment::MainEquipment: AlredySelectedCards = Save->Decks[Save->ActiveDeckIndex].MainEquipment;
			break;
		case EDeckSegment::SecondaryEquipment: AlredySelectedCards = Save->Decks[Save->ActiveDeckIndex].SecondaryEquipment;
			break;
		}
		CardSelector->ConfigCardSelector(Segment, DefinitionPtr->Size, AlredySelectedCards);
		CardSelector->FillCards(CardNames, CardIcons, CardColors, CardIndexes);
		CardSelector->SetTitle(DefinitionPtr->Messege);
		CardSelector->OnNeededCardsSelected.AddDynamic(this, &APlayerCustomiser::UpdateDeck);

		PlayerCustomisingWidget->OpenCardSelector(CardSelector);
	}
}

void APlayerCustomiser::DisplayDeck(FDeckPersistentData Deck)
{
	OnDeckPlayable.Broadcast(IsDeckPlayable(Deck));
	DisplayDeckCards(Deck.Heroes, EDeckSegment::Hero);
	DisplayDeckCards(Deck.MainEquipment, EDeckSegment::MainEquipment);
	DisplayDeckCards(Deck.SecondaryEquipment, EDeckSegment::SecondaryEquipment);
}

void APlayerCustomiser::DisplayDeckCards(TArray<uint16> CardIndexes, EDeckSegment Segment)
{

	if (!IsValid(ResourceTable))
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

	FSegementDefinition* DefinitionPtr = SegmentDefinitions.Find(Segment);
	if (DefinitionPtr == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::DisplayDeckCards Segment %s is not propperly defined check SegmentDefinitions property"), *UEnum::GetValueAsString(Segment));
		return;
	}

	TArray<FText> CardNames;
	TArray<UTexture2D*> CardIcons;
	TArray<FLinearColor> CardColors;

	TMap<uint16, FResourseCardPersistentData*> SavedCards = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, ResourceTable, DefinitionPtr->Series);

	for (int i = 0; i < SegmentDefinitions.Find(Segment)->Size; i++)
	{
		FResourseCardPersistentData** CardPtr;
		FResourseCardPersistentData Card;
		if (i < CardIndexes.Num())
		{
			CardPtr = SavedCards.Find(CardIndexes[i]);
			if (CardPtr == nullptr || *CardPtr == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("APlayerCustomiser::DisplayDeckCards Could not find card on Index %d"), CardIndexes[i]);
				Card = UDeckFunctionLibrary::GenerateCardWithID(ResourceTable, 0);
			}
			else
			{
				Card = **CardPtr;
			}
		}
		else
		{
			Card = UDeckFunctionLibrary::GenerateCardWithID(ResourceTable, 0);
		}

		CardNames.Add(Card.UnitName);
		CardIcons.Add(Card.UnitImage);
		CardColors.Add(Card.UnitColor);
	}

	 (*SegmentPanels.Find(Segment))->FillSegment(CardNames, CardIcons, CardColors);

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

void APlayerCustomiser::UpdateDeck(TArray<uint16> CardIndexes, EDeckSegment SegmentToUpdate)
{
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::UpdateDeck IsValid(InteractingPlayer) == false"));
		return;
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::UpdateDeck IsValid(Save) == false"));
		return;
	}

	FDeckPersistentData ActiveDeck = Save->Decks[Save->ActiveDeckIndex];

	switch (SegmentToUpdate)
	{
	case EDeckSegment::Hero:
		ActiveDeck.Heroes = CardIndexes;
		for (uint16 Card : CardIndexes)
		{
			ActiveDeck.MainEquipment.Remove(Card);
			ActiveDeck.SecondaryEquipment.Remove(Card);
		}
		break;

	case EDeckSegment::MainEquipment:
		ActiveDeck.MainEquipment = CardIndexes;
		for (uint16 Card : CardIndexes)
		{
			ActiveDeck.Heroes.Remove(Card);
			ActiveDeck.SecondaryEquipment.Remove(Card);
		}
		break;

	case EDeckSegment::SecondaryEquipment:
		ActiveDeck.SecondaryEquipment = CardIndexes;
		for (uint16 Card : CardIndexes)
		{
			ActiveDeck.MainEquipment.Remove(Card);
			ActiveDeck.Heroes.Remove(Card);
		}
		break;
	}

	Save->Decks[Save->ActiveDeckIndex] = ActiveDeck;

	InteractingPlayer->WriteSave();

	DisplayDeck(ActiveDeck);
}

void APlayerCustomiser::CleanseDeck()
{
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::CleanseDeck IsValid(InteractingPlayer) == false"));
		return;
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::CleanseDeck IsValid(Save) == false"));
		return;
	}

	TMap<uint16, FResourseCardPersistentData*> SavedCards = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, ResourceTable, TArray<uint16>());

	FDeckPersistentData ActiveDeck = Save->Decks[Save->ActiveDeckIndex];

	//Cleasing Hero Cards
	TArray<int> IndexesToRemove;
	for (int i = 0; i < ActiveDeck.Heroes.Num(); i++)
	{
		if (SavedCards.Contains(ActiveDeck.Heroes[i]) == false)
		{
			IndexesToRemove.Add(i);
		}
	}
	
	for (int i : IndexesToRemove)
	{
		ActiveDeck.Heroes.RemoveAt(i);  //Very costly but aaaaaah
	}
	
	IndexesToRemove.Empty();

	//Cleasing Main Cards
	for (int i = 0; i < ActiveDeck.MainEquipment.Num(); i++)
	{
		if (SavedCards.Contains(ActiveDeck.MainEquipment[i]) == false)
		{
			IndexesToRemove.Add(i);
		}
	}
	
	for (int i : IndexesToRemove)
	{
		ActiveDeck.MainEquipment.RemoveAt(i);  //Very costly but aaaaaah
	}
	
	IndexesToRemove.Empty();

	//Cleasing Secondary Cards
	for (int i = 0; i < ActiveDeck.SecondaryEquipment.Num(); i++)
	{
		if (SavedCards.Contains(ActiveDeck.SecondaryEquipment[i]) == false)
		{
			IndexesToRemove.Add(i);
		}
	}
	
	for (int i : IndexesToRemove)
	{
		ActiveDeck.SecondaryEquipment.RemoveAt(i);  //Very costly but aaaaaah
	}
	
	IndexesToRemove.Empty();

	Save->Decks[Save->ActiveDeckIndex] = ActiveDeck;

	InteractingPlayer->WriteSave();

}

bool APlayerCustomiser::IsDeckPlayable(FDeckPersistentData Deck)
{

	FSegementDefinition* HeroSegmentPtr = SegmentDefinitions.Find(EDeckSegment::Hero);
	FSegementDefinition* MainSegmentPtr = SegmentDefinitions.Find(EDeckSegment::MainEquipment);
	FSegementDefinition* SecondarySegmentPtr = SegmentDefinitions.Find(EDeckSegment::SecondaryEquipment);

	if (HeroSegmentPtr == nullptr || MainSegmentPtr == nullptr || SecondarySegmentPtr == nullptr) return false;

	return Deck.Heroes.Num() == HeroSegmentPtr->Size && Deck.MainEquipment.Num() == MainSegmentPtr->Size && Deck.SecondaryEquipment.Num() == SecondarySegmentPtr->Size;
}

void APlayerCustomiser::SendDeckData()
{
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::CleanseDeck IsValid(InteractingPlayer) == false"));
		return;
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::CleanseDeck IsValid(Save) == false"));
		return;
	}
	TArray<UTexture2D*> DeckIcons;

	for (int i = 0; i < Save->UnlockedDecks; i++)
	{
		DeckIcons.Add(Save->Decks[i].UnitImage);
	}
	PlayerCustomisingWidget->InitDecks(Save->ActiveDeckIndex, DeckIcons);

}

void APlayerCustomiser::UseDeck()
{
	CleanseDeck();

	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::UseDeck IsValid(InteractingPlayer) == false"));
		return;
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::UseDeck IsValid(Save) == false"));
		return;
	}

	FDeckPersistentData ActiveDeck = Save->Decks[Save->ActiveDeckIndex];

	DisplayDeck(ActiveDeck);
}

void APlayerCustomiser::SwitchDeck(int DeckIndex)
{
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::UseDeck IsValid(InteractingPlayer) == false"));
		return;
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::UseDeck IsValid(Save) == false"));
		return;
	}

	//if (DeckIndex >= Save->UnlockedDecks) DeckIndex -= Save->UnlockedDecks;
	//else if (DeckIndex < 0) DeckIndex += Save->UnlockedDecks;

	Save->ActiveDeckIndex = DeckIndex;

	UseDeck();
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

	PlayerCustomisingWidget->AddToViewport();
}

void APlayerCustomiser::Client_ShowUI_Implementation()
{
	ShowUI();
}


