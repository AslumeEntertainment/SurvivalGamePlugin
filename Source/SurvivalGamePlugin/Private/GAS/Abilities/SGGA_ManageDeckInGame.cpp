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

	PlayerCont->SetInputMode(FInputModeGameAndUI());
	PlayerCont->bShowMouseCursor = true;

	InGameDeckWidget->AddToViewport();
	InGameDeckWidget->OnCardDropped.BindDynamic(this, &USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToWorld);
	
	InGameDeckWidget->GetEquipedCardsContainer()->OnCardDropped.BindDynamic(this, &USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToEquipment);
	InGameDeckWidget->GetEquipedCardsContainer()->InitContainer(EquipmentComp->GetEquipmentSlots().Num());
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
		if (i >= MainResourceDataRows.Num())
		{
			continue;
		}
		MainItemsData.Add(MainResourceDataRows[i]);
	}
	InGameDeckWidget->GetMainCardsContainer()->InitContainer(3);
	RefreshMainCards();
	
	TArray<FResourseCardPersistentData*> SecondaryResourceDataRows = UDeckFunctionLibrary::GetPlayerResourceSaveData(Save, SecondaryHandResourceTable);
	for (int i = 0; i < 3; i++)
	{
		if (i >= SecondaryResourceDataRows.Num())
		{
			continue;
		}
		SecondaryItemsData.Add(SecondaryResourceDataRows[i]);
	}
	InGameDeckWidget->GetSecondaryCardsContainer()->InitContainer(3);
	RefreshSecondaryCards();
}

void USGGA_ManageDeckInGame::RefreshEquipedCards()
{
	InGameDeckWidget->GetEquipedCardsContainer()->ClearCards();
	EquipedItemsData.Empty();

	UUnitIconWidget* ItemIcon = nullptr;

	TArray<AItemBase*> EquipedItems;  
	if (Player->IsRightHanded())
	{
		EquipedItems = EquipmentComp->GetAllEquipedItems(true, true);
	}
	else
	{
		EquipedItems = EquipmentComp->GetAllEquipedItems(false, true);
	}

	for (int i = 0; i < EquipedItems.Num(); i++)
	{
		if (IsValid(EquipedItems[i]) == false)
		{
			EquipedItemsData.Add(nullptr);
			continue;
		}
		FResourseCardPersistentData* ItemCardData = EquipedItems[i]->ItemData.ResourceCardData.GetRow<FResourseCardPersistentData>("");
		if (ItemCardData == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_ManageDeckInGame::OpenDeckManager ItemCardData == nullptr"));
			continue;
		}

		ItemIcon = InGameDeckWidget->GetEquipedCardsContainer()->InsertCardAt(ItemCardData->UnitName,
			ItemCardData->UnitImage, ItemCardData->UnitColor, i);

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

	for (int i = 0; i < MainItemsData.Num(); i++)
	{
		if (MainItemsData[i] == nullptr)
		{
			continue;
		}
		ItemIcon = InGameDeckWidget->GetMainCardsContainer()->InsertCardAt(MainItemsData[i]->UnitName, MainItemsData[i]->UnitImage, MainItemsData[i]->UnitColor, i);

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

	for (int i = 0; i < SecondaryItemsData.Num(); i++)
	{
		if (SecondaryItemsData[i] == nullptr)
		{
			continue;
		}
		ItemIcon = InGameDeckWidget->GetSecondaryCardsContainer()->InsertCardAt(SecondaryItemsData[i]->UnitName, SecondaryItemsData[i]->UnitImage, SecondaryItemsData[i]->UnitColor, i);

		if (IsValid(ItemIcon))
		{
			ItemIcon->SetIsDraggable(true);
		}
	}
}

void USGGA_ManageDeckInGame::CloseDeckManager(float TimeHeld)
{
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

bool USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToWorld(UCardSlotWidget* Slot, int CardIndex)
{
	if (bIsDropedItemToBeEquiped)
	{
		return false;
	}
	
	if (InGameDeckWidget->GetEquipedCardsContainer()->ContainsSlot(Slot))
	{
		if (Player->IsRightHanded())
		{
			EquipmentComp->UnequipItem(EquipmentComp->GetAllEquipedItems(true, true)[CardIndex]);
		}
		else
		{
			EquipmentComp->UnequipItem(EquipmentComp->GetAllEquipedItems(false, true)[CardIndex]);
		}
		RefreshEquipedCards();
		return true;
	}

	FResourseCardPersistentData* ResourseCardData = nullptr;

	if (InGameDeckWidget->GetMainCardsContainer()->ContainsSlot(Slot))
	{
		ResourseCardData = MainItemsData[CardIndex];
	}
	if (InGameDeckWidget->GetSecondaryCardsContainer()->ContainsSlot(Slot))
	{
		ResourseCardData = SecondaryItemsData[CardIndex];
	}
	if (ResourseCardData == nullptr)
	{
		return false;
	}

	FItemDataStruct* ResourceItemData = ResourseCardData->RepresentedItem.GetRow<FItemDataStruct>("");

	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Attempting to drop %s"), *ResourseCardData->UnitName.ToString()));

	SpawnItemFromCard(false, ResourseCardData->RepresentedItemClass, *ResourceItemData);
	
	if (InGameDeckWidget->GetMainCardsContainer()->ContainsSlot(Slot))
	{
		MainItemsData[CardIndex] = nullptr;
	}
	if (InGameDeckWidget->GetSecondaryCardsContainer()->ContainsSlot(Slot))
	{
		SecondaryItemsData[CardIndex] = nullptr;
	}
	
	return true;
}

bool USGGA_ManageDeckInGame::AttemptTransferCardFromDeckToEquipment(UCardSlotWidget* Slot, int CardIndex)
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

	FResourseCardPersistentData* ResourseCardData = nullptr;

	if (InGameDeckWidget->GetMainCardsContainer()->ContainsSlot(Slot))
	{
		ResourseCardData = MainItemsData[CardIndex];
	}
	else if (InGameDeckWidget->GetSecondaryCardsContainer()->ContainsSlot(Slot))
	{
		ResourseCardData = SecondaryItemsData[CardIndex];
	}
	if (ResourseCardData == nullptr)
	{
		return false;
	}

	FItemDataStruct* ResourceItemData = ResourseCardData->RepresentedItem.GetRow<FItemDataStruct>("");
	FGameplayTag ItemEquipmentSlot = ResourceItemData->EquipmentSlotTag;

	if (EquipmentComp->HasEquipmentSlotWithTag(ItemEquipmentSlot) == false && EquipmentComp->HasEquipmentSlotWithTag(EquipmentComp->GetMixedEquipmentSlotTag()) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttemptTransferCardFromDeckToEquipment: Player doesn't have an equipment slot for this item"));
		return false;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Attempting to equip %s"), *ResourseCardData->UnitName.ToString()));

	SpawnItemFromCard(true, ResourseCardData->RepresentedItemClass, *ResourceItemData);
	if (InGameDeckWidget->GetMainCardsContainer()->ContainsSlot(Slot))
	{
		MainItemsData[CardIndex] = nullptr;
	}
	else if (InGameDeckWidget->GetSecondaryCardsContainer()->ContainsSlot(Slot))
	{
		SecondaryItemsData[CardIndex] = nullptr;
	}
	RefreshEquipedCards();

	return true;
}

void USGGA_ManageDeckInGame::SpawnItemFromCard(bool bShouldEquip, TSubclassOf<AItemBase> ItemClass, FItemDataStruct ItemData)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		//Server_SpawnItemFromCard(bShouldEquip, ItemClass, ItemData);
		//return;
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
		EquipmentComp->EquipItem(NewItem);
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

