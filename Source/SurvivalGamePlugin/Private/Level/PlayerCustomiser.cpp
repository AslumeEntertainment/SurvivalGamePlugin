// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/PlayerCustomiser.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h" 
#include "Net/UnrealNetwork.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
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
		TArray<FHeroCardPersistentData*> HeroDataRows;
		HeroTable->GetAllRows<FHeroCardPersistentData>("Filling Heroes", HeroDataRows);

		for (FProgressiveDataHelperStruct SavedHero : Save->HeroCards)
		{
			for (FHeroCardPersistentData* HeroRow : HeroDataRows)
			{
				if (SavedHero.ID != HeroRow->ID) continue;

				CardNames.Add(HeroRow->UnitName);
				CardIcons.Add(HeroRow->UnitImage);
				CardColors.Add(HeroRow->UnitColor);
			}
		}
		CardsNeeded = 1;
		Title = "Select a hero card";
	}
	if (CallerWidget == PlayerCustomisingWidget->GetMainCardsPanel())
	{
		TArray<FResourseCardPersistentData*> MainResourceDataRows;
		MainHandResourceTable->GetAllRows<FResourseCardPersistentData>("Filling main hand resources", MainResourceDataRows);

		for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
		{
			for (FResourseCardPersistentData* ResourceRow : MainResourceDataRows)
			{
				if (SavedResource.ID != ResourceRow->ID) continue;

				CardNames.Add(ResourceRow->UnitName);
				CardIcons.Add(ResourceRow->UnitImage);
				CardColors.Add(ResourceRow->UnitColor);
			}
		}
		CardsNeeded = 3;
		Title = "Select main hand resource cards";
	}
	if (CallerWidget == PlayerCustomisingWidget->GetSecondaryCardsPanel())
	{
		TArray<FResourseCardPersistentData*> SecondaryResourceDataRows;
		SecondaryHandResourceTable->GetAllRows<FResourseCardPersistentData>("Filling secondary hand resources", SecondaryResourceDataRows);

		for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
		{
			for (FResourseCardPersistentData* ResourceRow : SecondaryResourceDataRows)
			{
				if (SavedResource.ID != ResourceRow->ID) continue;

				CardNames.Add(ResourceRow->UnitName);
				CardIcons.Add(ResourceRow->UnitImage);
				CardColors.Add(ResourceRow->UnitColor);
			}
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
		PlayerCustomisingWidget->OpenCardSelector(CardSelector);
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
		}
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerCustomiser::Client_ShowUI_Implementation IsValid(Save) == false"));
		return;
	}

	TArray<FHeroCardPersistentData*> HeroDataRows;
	HeroTable->GetAllRows<FHeroCardPersistentData>("Filling Heroes", HeroDataRows);

	FProgressiveDataHelperStruct FirstSavedHero = Save->HeroCards[0];
	for (FHeroCardPersistentData* HeroRow : HeroDataRows)
	{
		if (FirstSavedHero.ID == HeroRow->ID)
		{
			UUnitIconWidget* Card = PlayerCustomisingWidget->FillHeroCardSlot(HeroRow->UnitName, HeroRow->UnitImage, HeroRow->UnitColor);
			if (IsValid(Card))
			{
				Card->BroadcastParentOnClicked.AddDynamic(this, &APlayerCustomiser::OpenCardSelector);
			}
			
			break;
		}
	}

	TArray<FText> ResourceNames;
	TArray<UTexture2D*> ResourceIcons;
	TArray<FLinearColor> ResourceColors;
	TArray<UUnitIconWidget*> Cards;

	TArray<FResourseCardPersistentData*> MainResourceDataRows;
	MainHandResourceTable->GetAllRows<FResourseCardPersistentData>("Filling main hand resources", MainResourceDataRows);

	for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
	{
		for (FResourseCardPersistentData* ResourceRow : MainResourceDataRows)
		{
			if (SavedResource.ID != ResourceRow->ID) continue;

			ResourceNames.Add(ResourceRow->UnitName);
			ResourceIcons.Add(ResourceRow->UnitImage);
			ResourceColors.Add(ResourceRow->UnitColor);

			if (ResourceNames.Num() == 3) break;
		}
		if (ResourceNames.Num() == 3) break;
	}
	Cards = PlayerCustomisingWidget->FillMainHandCardsSlots(ResourceNames, ResourceIcons, ResourceColors);
	for (UUnitIconWidget* Card : Cards)
	{
		if (IsValid(Card))
		{
			Card->BroadcastParentOnClicked.AddDynamic(this, &APlayerCustomiser::OpenCardSelector);
		}
	}

	ResourceNames.Empty();
	ResourceIcons.Empty();
	ResourceColors.Empty();

	TArray<FResourseCardPersistentData*> SecondaryResourceDataRows;
	SecondaryHandResourceTable->GetAllRows<FResourseCardPersistentData>("Filling secondary hand resources", SecondaryResourceDataRows);

	for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
	{
		for (FResourseCardPersistentData* ResourceRow : SecondaryResourceDataRows)
		{
			if (SavedResource.ID != ResourceRow->ID) continue;

			ResourceNames.Add(ResourceRow->UnitName);
			ResourceIcons.Add(ResourceRow->UnitImage);
			ResourceColors.Add(ResourceRow->UnitColor);

			if (ResourceNames.Num() == 3) break;
		}
		if (ResourceNames.Num() == 3) break;
	}
	Cards = PlayerCustomisingWidget->FillSecondaryHandCardsSlots(ResourceNames, ResourceIcons, ResourceColors);
	for (UUnitIconWidget* Card : Cards)
	{
		if (IsValid(Card))
		{
			Card->BroadcastParentOnClicked.AddDynamic(this, &APlayerCustomiser::OpenCardSelector);
		}
	}

	PlayerCustomisingWidget->AddToViewport();
}


