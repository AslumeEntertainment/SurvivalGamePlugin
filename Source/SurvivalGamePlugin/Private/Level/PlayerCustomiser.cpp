// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/PlayerCustomiser.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h" 
#include "Net/UnrealNetwork.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "FunctionLibraries/DeckFunctionLibrary.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "Characters/InGameCharacter.h"
#include "UI/Widgets/PlayerCustomisingWidget.h"
#include "UI/Widgets/Deck/CardSelectorWidget.h"
#include "UI/Widgets/UnitIconWidget.h"
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

void APlayerCustomiser::InteractRequest(AController* InteractingCharacter)
{
	UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest - %s"), *InteractingCharacter->GetName());
	ASG_PlayerController* PC = Cast<ASG_PlayerController>(InteractingCharacter);

	if (IsValid(PC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::InteractRequest IsValid(InteractingPlayer) == false"));
		return;
	}

	//this makes it possible to tell the server you are changing states aka to use RPCs
	SetOwner(InteractingCharacter);
	InteractingPlayer = PC;

	if (PC->IsLocalPlayerController() == true)
	{
		/*AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD());
		if (IsValid(HUD))
		{
			HUD->RemoveInteractionOverlayWidget();
		}*/

		PC->Client_SetInputMode(EInputMode::UIOnly);
		PC->bShowMouseCursor = true;
		Client_ShowUI();
	}

	if (HasAuthority() == true)
	{
		OnPlayerStartInteracting.Broadcast(this);
		MainPawn = PC->GetPawn();

		PC->OnConnectionCleanUp.AddDynamic(this, &APlayerCustomiser::HandlePlayerLeavingSession);

		bIsTaken = true;
	}
}

bool APlayerCustomiser::bCanInteract(AController* InteractingCharacter)
{
	return bIsActive && !bIsTaken && !PlayerCustomisingWidget->IsInViewport();
}

void APlayerCustomiser::Client_StopInteracting_Implementation()
{
	Client_HideUI(true);

	if (IsValid(InteractingPlayer) == true)
	{
		InteractingPlayer->SetViewTargetWithBlend(InteractingPlayer->GetPawn(), 0.25f, EViewTargetBlendFunction::VTBlend_Linear);
		InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
		InteractingPlayer->bShowMouseCursor = false;
		InteractingPlayer->OnConnectionCleanUp.RemoveAll(this);
		InteractingPlayer = nullptr;
	}

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
		TArray<FResourseCardPersistentData*> MainResourceDataRows = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, MainHandResourceTable);
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
		TArray<FResourseCardPersistentData*> SecondaryResourceDataRows = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, SecondaryHandResourceTable);
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

void APlayerCustomiser::DisplayDeckCards(TArray<int> CardIndexes, UPanelWidget* CardBox)
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

	TArray<FResourseCardPersistentData*> ResourceDataRows;

	if (CardBox == PlayerCustomisingWidget->GetMainCardsPanel())
	{
		ResourceDataRows = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, MainHandResourceTable);
	}
	else if (CardBox == PlayerCustomisingWidget->GetSecondaryCardsPanel())
	{
		ResourceDataRows = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, SecondaryHandResourceTable);
	}

	for (int i : CardIndexes)
	{
		if (i >= ResourceDataRows.Num() || ResourceDataRows[i] == nullptr)
		{
			continue;
		}
		if (CardBox == PlayerCustomisingWidget->GetMainCardsPanel())
		{
			TempCard = PlayerCustomisingWidget->FillSingleMainHandCardSlot(ResourceDataRows[i]->UnitName,
				ResourceDataRows[i]->UnitImage, ResourceDataRows[i]->UnitColor);
		}
		else if (CardBox == PlayerCustomisingWidget->GetSecondaryCardsPanel())
		{
			TempCard = PlayerCustomisingWidget->FillSingleSecondaryHandCardSlot(ResourceDataRows[i]->UnitName,
				ResourceDataRows[i]->UnitImage, ResourceDataRows[i]->UnitColor);
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
	HUD->ShowInteractionOverlayWidget();
}

void APlayerCustomiser::Client_HideUI_Implementation(bool bUsesAnimation)
{
	AInGameHUD* HUD = Cast<AInGameHUD>(InteractingPlayer->GetHUD());
	if (IsValid(HUD))
	{
		HUD->ShowInteractionOverlayWidget();
		InteractingPlayer->bShowMouseCursor = false;
	}

	bUsesAnimation ? PlayerCustomisingWidget->CloseWidget() : PlayerCustomisingWidget->RemoveFromParent();
}

void APlayerCustomiser::Client_ShowUI_Implementation()
{
	if (IsValid(InteractingPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Client_ShowUI_Implementation IsValid(InteractingPlayer) == false"));
		return;
	}

	if (!IsValid(HeroTable) || !IsValid(MainHandResourceTable) || !IsValid(SecondaryHandResourceTable))
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Client_ShowUI_Implementation One or more card data tables are invalid"));
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

	TArray<int> NeededHeroCards;
	NeededHeroCards.Add(0);

	TArray<int> NeededResourceCards;
	NeededResourceCards.Add(0);
	NeededResourceCards.Add(1);
	NeededResourceCards.Add(2);

	PlayerCustomisingWidget->ClearMainCardsPanel();
	PlayerCustomisingWidget->ClearSecondaryCardsPanel();

	DisplayDeckCards(NeededHeroCards, PlayerCustomisingWidget->GetHeroCardPanel());
	DisplayDeckCards(NeededResourceCards, PlayerCustomisingWidget->GetMainCardsPanel());
	DisplayDeckCards(NeededResourceCards, PlayerCustomisingWidget->GetSecondaryCardsPanel());

	PlayerCustomisingWidget->AddToViewport();
}


