// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ManageDeckInGame.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PanelWidget.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "FunctionLibraries/DeckFunctionLibrary.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "Components/EquipmentComponent.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"
#include "UI/Widgets/Deck/CardContainer.h"
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

	if (HasAuthority(&ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("SERVER Setting task"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CLIENT Setting task"));
	}

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

	PlayerCont = Cast<APlayerController>(Player->GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager IsValid(PlayerCont) == false"));
		return;
	}

	InGameDeckWidget->AddToViewport();
	PlayerCont->SetInputMode(FInputModeGameAndUI());
	PlayerCont->bShowMouseCursor = true;

	InGameDeckWidget->OnCardDropped.BindDynamic(this, &USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToWorld);
	InGameDeckWidget->GetEquipedCardsContainer()->OnCardDropped.BindDynamic(this, &USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToEquipment);

	RefreshEquipedCards();

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

	//TO BE CHANGED WHEN DECKS WORK

	TArray<FResourseCardPersistentData*> MainResourceDataRows = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, MainHandResourceTable);
	for (int i = 0; i < 3; i++)
	{
		if (i >= MainResourceDataRows.Num() || MainResourceDataRows[i] == nullptr)
		{
			continue;
		}
		MainItemsData.Add(MainResourceDataRows[i]);
	}
	RefreshMainCards();
	
	TArray<FResourseCardPersistentData*> SecondaryResourceDataRows = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, SecondaryHandResourceTable);
	for (int i = 0; i < 3; i++)
	{
		if (i >= SecondaryResourceDataRows.Num() || SecondaryResourceDataRows[i] == nullptr)
		{
			continue;
		}
		SecondaryItemsData.Add(SecondaryResourceDataRows[i]);
	}
	RefreshSecondaryCards();
}

void USGGA_ManageDeckInGame::RefreshEquipedCards()
{
	InGameDeckWidget->GetEquipedCardsContainer()->ClearCards();

	UUnitIconWidget* ItemIcon = nullptr;

	TArray<AItemBase*> EquipedItems = EquipmentComp->GetAllEquipedItems();
	for (int i = EquipedItems.Num() - 1; i >= 0; i--)
	{
		FResourseCardPersistentData* ItemCardData = EquipedItems[i]->ItemData.ResourceCardData.GetRow<FResourseCardPersistentData>("");
		if (ItemCardData == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager ItemCardData == nullptr"));
			continue;
		}

		ItemIcon = InGameDeckWidget->GetEquipedCardsContainer()->AddCard(ItemCardData->UnitName,
			ItemCardData->UnitImage, ItemCardData->UnitColor);

		if (IsValid(ItemIcon))
		{
			ItemIcon->SetIsDraggable(true);
		}
		EquipedItemsData.Add(ItemCardData);
	}
}

void USGGA_ManageDeckInGame::RefreshMainCards()
{
	InGameDeckWidget->GetMainCardsContainer()->ClearCards();

	UUnitIconWidget* ItemIcon = nullptr;

	for (FResourseCardPersistentData* ItemData : MainItemsData)
	{
		ItemIcon = InGameDeckWidget->GetMainCardsContainer()->AddCard(ItemData->UnitName, ItemData->UnitImage, ItemData->UnitColor);

		if (IsValid(ItemIcon))
		{
			ItemIcon->SetIsDraggable(true);
		}
	}
}

void USGGA_ManageDeckInGame::RefreshSecondaryCards()
{
	InGameDeckWidget->GetSecondaryCardsContainer()->ClearCards();

	UUnitIconWidget* ItemIcon = nullptr;

	for (FResourseCardPersistentData* ItemData : SecondaryItemsData)
	{
		ItemIcon = InGameDeckWidget->GetSecondaryCardsContainer()->AddCard(ItemData->UnitName, ItemData->UnitImage, ItemData->UnitColor);

		if (IsValid(ItemIcon))
		{
			ItemIcon->SetIsDraggable(true);
		}
	}
}

void USGGA_ManageDeckInGame::CloseDeckManager(float TimeHeld)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("SERVER Task executing"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CLIENT Task executing"));
	}

	if (IsLocallyControlled() == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	UWidgetBlueprintLibrary::CancelDragDrop();
	InGameDeckWidget->RemoveFromParent();

	PlayerCont->SetInputMode(FInputModeGameOnly());
	PlayerCont->bShowMouseCursor = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToWorld(UWidget* CardParent, int CardIndex)
{
	if (bIsDropedItemToBeEquiped)
	{
		return false;
	}

	FResourseCardPersistentData* ResourseCardData = nullptr;

	if (InGameDeckWidget->GetMainCardsContainer()->IsThisMyCardPanel(CardParent))
	{
		ResourseCardData = MainItemsData[CardIndex];
	}
	else if (InGameDeckWidget->GetSecondaryCardsContainer()->IsThisMyCardPanel(CardParent))
	{
		ResourseCardData = SecondaryItemsData[CardIndex];
	}
	if (ResourseCardData == nullptr)
	{
		return false;
	}

	FItemDataStruct* ResourceItemData = ResourseCardData->RepresentedItem.GetRow<FItemDataStruct>("");

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Attempting to drop %s"), *ResourseCardData->UnitName.ToString()));

	SpawnItemFromCard(false, ResourseCardData->RepresentedItemClass, *ResourceItemData);
	if (InGameDeckWidget->GetMainCardsContainer()->IsThisMyCardPanel(CardParent))
	{
		MainItemsData.RemoveAt(CardIndex);
		RefreshMainCards();
	}
	else if (InGameDeckWidget->GetSecondaryCardsContainer()->IsThisMyCardPanel(CardParent))
	{
		SecondaryItemsData.RemoveAt(CardIndex);
		RefreshSecondaryCards();
	}
	return true;
}

bool USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToEquipment(UWidget* CardParent, int CardIndex)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToEquipment IsValid(World) == false"));
		return false;
	}

	bIsDropedItemToBeEquiped = true;
	FTimerHandle EnableDroppingInWorldTimer;
	World->GetTimerManager().SetTimer(EnableDroppingInWorldTimer, this, &USGGA_ManageDeckInGame::EnableDroppingInWorld, 0.05f);

	UCardContainer* CardParentContainer = nullptr;
	FResourseCardPersistentData* ResourseCardData = nullptr;

	if (InGameDeckWidget->GetMainCardsContainer()->IsThisMyCardPanel(CardParent))
	{
		ResourseCardData = MainItemsData[CardIndex];
		CardParentContainer = InGameDeckWidget->GetMainCardsContainer();
	}
	else if (InGameDeckWidget->GetSecondaryCardsContainer()->IsThisMyCardPanel(CardParent))
	{
		ResourseCardData = SecondaryItemsData[CardIndex];
		CardParentContainer = InGameDeckWidget->GetSecondaryCardsContainer();
	}
	if (ResourseCardData == nullptr)
	{
		return false;
	}

	FItemDataStruct* ResourceItemData = ResourseCardData->RepresentedItem.GetRow<FItemDataStruct>("");
	FGameplayTag ItemEquipmentSlot = ResourceItemData->EquipmentSlotTag;

	if (EquipmentComp->HasEquipmentSlotWithTag(ItemEquipmentSlot) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttemptTransferCardFromDeckToEquipment: Player doesn't have an equipment slot for this item"));
		return false;
	}
	if (IsValid(EquipmentComp->GetEquipedItemAtSlot(ItemEquipmentSlot)))
	{
		UE_LOG(LogTemp, Warning, TEXT("AttemptTransferCardFromDeckToEquipment: Player doesn't have space for this item"));
		return false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Attempting to equip %s"), *ResourseCardData->UnitName.ToString()));

	SpawnItemFromCard(true, ResourseCardData->RepresentedItemClass, *ResourceItemData);
	if (InGameDeckWidget->GetMainCardsContainer()->IsThisMyCardPanel(CardParent))
	{
		MainItemsData.RemoveAt(CardIndex);
		RefreshMainCards();
	}
	else if (InGameDeckWidget->GetSecondaryCardsContainer()->IsThisMyCardPanel(CardParent))
	{
		SecondaryItemsData.RemoveAt(CardIndex);
		RefreshSecondaryCards();
	}
	RefreshEquipedCards();

	return true;
}

void USGGA_ManageDeckInGame::SpawnItemFromCard(bool bShouldEquip, TSubclassOf<AItemBase> ItemClass, FItemDataStruct ItemData)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		Server_SpawnItemFromCard(bShouldEquip, ItemClass, ItemData);
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::SpawnItemFromCard IsValid(World) == false"));
		return;
	}

	AItemBase* NewItem = World->SpawnActor<AItemBase>(ItemClass, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());

	if (IsValid(NewItem) && bShouldEquip)
	{
		EquipmentComp->SetEquipedItemAtSlot(ItemData.EquipmentSlotTag, NewItem);
		NewItem->OnItemEquiped(Player, EquipmentComp->GetEquipmentSocketWithTag(ItemData.EquipmentSlotTag));
	}
}

void USGGA_ManageDeckInGame::Server_SpawnItemFromCard_Implementation(bool bShouldEquip, TSubclassOf<AItemBase> ItemClass, FItemDataStruct ItemData)
{
	SpawnItemFromCard(bShouldEquip, ItemClass, ItemData);
}

void USGGA_ManageDeckInGame::EnableDroppingInWorld()
{
	bIsDropedItemToBeEquiped = false;
}

