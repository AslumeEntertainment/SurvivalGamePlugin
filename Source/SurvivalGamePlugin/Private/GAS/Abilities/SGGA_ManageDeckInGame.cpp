// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ManageDeckInGame.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Kismet/GameplayStatics.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "Components/EquipmentComponent.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"
#include "UI/Widgets/Deck/InGameDeck.h"
#include "UI/Widgets/UnitIconWidget.h"


void USGGA_ManageDeckInGame::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	OpenDeckManager();

	WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_ManageDeckInGame::CloseDeckManager);
	WaitInputReleaseTask->ReadyForActivation();
}

void USGGA_ManageDeckInGame::OpenDeckManager()
{
	if (IsLocallyControlled() == false)
	{
		return;
	}

	if (IsValid(MainHandResourceTable) == false || IsValid(SecondaryHandResourceTable) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager One or more invalid resource data tables!"));
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager IsValid(World) == false"));
		return;
	}

	InGameDeckWidget = CreateWidget<UInGameDeck>(UGameplayStatics::GetGameInstance(World), InGameDeckWidgetClass);
	UUnitIconWidget* ItemIcon = nullptr;

	PlayerCont = Cast<APlayerController>(Player->GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager IsValid(PlayerCont) == false"));
		return;
	}

	InGameDeckWidget->AddToViewport();
	PlayerCont->SetInputMode(FInputModeGameAndUI());
	PlayerCont->bShowMouseCursor = true;

	TArray<AItemBase*> EquipedItems = EquipmentComp->GetAllEquipedItems();
	for ( int i = EquipedItems.Num() - 1; i >= 0; i--)
	{
		FResourseCardPersistentData* ItemCardData = EquipedItems[i]->ItemData.ResourceCardData.GetRow<FResourseCardPersistentData>("");
		if (ItemCardData == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager ItemCardData == nullptr"));
			continue;
		}

		ItemIcon = CreateWidget<UUnitIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), ItemIconWidgetClass);
		ItemIcon->SetItemImage(ItemCardData->UnitImage);
		ItemIcon->SetItemName(ItemCardData->UnitName);
		ItemIcon->SetItemColor(ItemCardData->UnitColor);
		ItemIcon->SetIsDraggable(true);

		InGameDeckWidget->AddUnitCardToEquipedCardsBox(ItemIcon);
		InGameDeckWidget->SetEquipedCardsWidgetSwitcherActiveIndex(1);
	}

	ASG_PlayerController* SG_PlayerCont = Cast<ASG_PlayerController>(PlayerCont);
	if (IsValid(SG_PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager IsValid(SG_PlayerCont) == false"));
		return;
	}
	USG_SaveGame* Save = SG_PlayerCont->GetSave();
	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager IsValid(Save) == false"));
		return;
	}

	int Counter = 0;

	TArray<FResourseCardPersistentData*> MainResourceDataRows;
	MainHandResourceTable->GetAllRows<FResourseCardPersistentData>("Filling main hand resources", MainResourceDataRows);

	for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
	{
		for (FResourseCardPersistentData* ItemCardData : MainResourceDataRows)
		{
			if (SavedResource.ID != ItemCardData->ID) continue;

			ItemIcon = CreateWidget<UUnitIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), ItemIconWidgetClass);
			ItemIcon->SetItemImage(ItemCardData->UnitImage);
			ItemIcon->SetItemName(ItemCardData->UnitName);
			ItemIcon->SetItemColor(ItemCardData->UnitColor);
			ItemIcon->SetIsDraggable(true);

			InGameDeckWidget->AddUnitCardToMainCardsBox(ItemIcon);
			Counter++;

			if (Counter == 3) break;
		}
		if (Counter == 3) break;
	}

	Counter = 0;

	TArray<FResourseCardPersistentData*> SecondaryResourceDataRows;
	SecondaryHandResourceTable->GetAllRows<FResourseCardPersistentData>("Filling secondary hand resources", SecondaryResourceDataRows);

	for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
	{
		for (FResourseCardPersistentData* ItemCardData : SecondaryResourceDataRows)
		{
			if (SavedResource.ID != ItemCardData->ID) continue;

			ItemIcon = CreateWidget<UUnitIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), ItemIconWidgetClass);
			ItemIcon->SetItemImage(ItemCardData->UnitImage);
			ItemIcon->SetItemName(ItemCardData->UnitName);
			ItemIcon->SetItemColor(ItemCardData->UnitColor);
			ItemIcon->SetIsDraggable(true);

			InGameDeckWidget->AddUnitCardToSecondaryCardsBox(ItemIcon);
			Counter++;

			if (Counter == 3) break;
		}
		if (Counter == 3) break;
	}
}

void USGGA_ManageDeckInGame::CloseDeckManager(float TimeHeld)
{
	if (IsLocallyControlled() == false)
	{
		return;
	}

	InGameDeckWidget->RemoveFromParent();

	PlayerCont->SetInputMode(FInputModeGameOnly());
	PlayerCont->bShowMouseCursor = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
