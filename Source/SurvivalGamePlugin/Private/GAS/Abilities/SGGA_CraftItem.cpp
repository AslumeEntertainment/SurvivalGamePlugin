// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_CraftItem.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_GameplayAbilityTargetData.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "UI/Widgets/Game/CraftingWindowWidget.h"
#include "UI/Widgets/Core/UnitIconWidget.h"
#include "UI/HUD/InGameHUD.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "Components/EquipmentComponent.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"
#include "Animation/SG_AnimInstance.h"


void USGGA_CraftItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	OpenCraftingWindow();

	WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_CraftItem::CancelCrafting);
	WaitInputReleaseTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitStartCraftingTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CraftingProcessContinueTag, nullptr, true, true);
	WaitStartCraftingTask->EventReceived.AddDynamic(this, &USGGA_CraftItem::StartCrafting);
	WaitStartCraftingTask->ReadyForActivation();
}

void USGGA_CraftItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bWasCancelled && IsValid(ProductItem) && ProductItem->GetActorScale() == FVector(0.0001, 0.0001, 0.0001))
	{
		ProductItem->Destroy();
	}
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(SpeedReductionEffectSpecHandle);
	UE_LOG(LogTemp, Warning, TEXT("Crafting: EndAbility - Speed has been reset"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_CraftItem::OpenCraftingWindow()
{
	if (IsLocallyControlled() == false)
	{
		return;
	}

	PlayerCont = Cast<APlayerController>(Player->GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::OpenCraftingWindow IsValid(PlayerCont) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	AInGameHUD* HUD = PlayerCont->GetHUD<AInGameHUD>();
	if (IsValid(HUD) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::OpenCraftingWindow IsValid(HUD) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	if (HUD->CanOpenMoreMenus() == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	CraftingWindow = CreateWidget<UCraftingWindowWidget>(PlayerCont, CraftingWindowWidgetClass);
	CraftingWindow->AddToViewport();

	HUD->SetCanOpenMoreMenus(false);

	PlayerCont->SetInputMode(FInputModeGameAndUI());
	PlayerCont->bShowMouseCursor = true;

	AItemBase* MainItem = EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag());
	if (IsValid(MainItem) == false)
	{
		return;
	}
	AItemBase* SecondaryItem = EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag());
	if (IsValid(SecondaryItem) == false)
	{
		return;
	}

	TArray<FItemCraftingDataStruct> CraftingRecipes = MainItem->ItemData.CraftableItems;
	for (FItemCraftingDataStruct CraftingRecipe : CraftingRecipes)
	{
		if (CraftingRecipe.IngredientItemID == SecondaryItem->GetItemDataRowName() == false)
		{
			continue;
		}
		const FResourseCardPersistentData* ResourceCardData = CraftingRecipe.ProductItemResourceCardData.GetRow<FResourseCardPersistentData>("");
		if (ResourceCardData == nullptr)
		{
			continue;
		}

		PossibleProductItemNames.Add(CraftingRecipe.ProductItemResourceCardData.RowName);

		UUnitIconWidget* ItemIcon = CreateWidget<UUnitIconWidget>(PlayerCont, ItemIconWidgetClass);
		ItemIcon->SetItemImage(ResourceCardData->UnitImage);
		ItemIcon->SetItemName(ResourceCardData->UnitName);
		ItemIcon->SetItemColor(ResourceCardData->UnitColor);
		ItemIcon->IconClicked.AddDynamic(this, &USGGA_CraftItem::OnItemToCraftSelected);

		CraftingWindow->AddCraftingRecipe(ItemIcon);
		CraftingWindow->SetWidgetSwitcherActiveIndex(1);
	}
}

void USGGA_CraftItem::CloseCraftingWindow()
{
	if (IsLocallyControlled() == false)
	{
		return;
	}
	AInGameHUD* HUD = PlayerCont->GetHUD<AInGameHUD>();
	if (IsValid(HUD) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::CloseCraftingWindow IsValid(HUD) == false"));
		return;
	}
	
	CraftingWindow->RemoveFromParent();
	CraftingWindow->ClearRecipes();

	PlayerCont->SetInputMode(FInputModeGameOnly());
	PlayerCont->bShowMouseCursor = false;

	HUD->SetCanOpenMoreMenus(true);
}

void USGGA_CraftItem::CancelCrafting(float TimeHeld)
{
	CloseCraftingWindow();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_CraftItem::OnItemToCraftSelected(int IconID)
{
	CloseCraftingWindow();

	FSG_GameplayAbilityTargetData* TargetData = new FSG_GameplayAbilityTargetData();
	TargetData->OptionalNameProperty = PossibleProductItemNames[IconID];
	
	FGameplayEventData EventData = FGameplayEventData();
	EventData.TargetData.Add(TargetData);

	Player->SendGameplayEventToSelf(CraftingProcessContinueTag, EventData);
}

void USGGA_CraftItem::StartCrafting(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem: Starting to craft!"));

	WaitInputReleaseTask->EndTask();

	if (!IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())) || !IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::StartCrafting IsValid(Player->MainHandItem || Player->SecondaryHandItem) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FName ProductItemName = FName("None");
	if (Payload.TargetData.IsValid(0))
	{
		FSG_GameplayAbilityTargetData* MyTargetData = static_cast<FSG_GameplayAbilityTargetData*>(Payload.TargetData.Data[0].Get());
		if (MyTargetData)
		{
			ProductItemName = MyTargetData->OptionalNameProperty;
		}
	}

	ProductItemData = CraftingInfoTable->FindRow<FCraftingProcessDataStruct>(ProductItemName, "");
	if (ProductItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility ProductItemData == nullptr"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FGameplayEffectSpecHandle EffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(SpeedReductionEffect, 1, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();
	EffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("GameplayEffectCaller.Speed"), ProductItemData->MovementSpeedMultiplier);
	SpeedReductionEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec);

	Player->PlayAnimMontage(ProductItemData->Animation);
	GetWorld()->GetTimerManager().SetTimer(CraftTimer, this, &USGGA_CraftItem::OnCraftingFinished, ProductItemData->ProcessLength, false);
}

void USGGA_CraftItem::OnCraftingFinished()
{
	float ProductItemDurabilityPercent = 0.f;

	if (ProductItemData->ItemsToDestroy.Contains(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())->GetItemDataRowName()))
	{
		ProductItemSocketName = EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetSecondaryEquipmentSlotTag());
		ProductItemDurabilityPercent += EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())->GetDurability() /
			EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())->ItemData.ItemMaxDurability;
	}
	if (ProductItemData->ItemsToDestroy.Contains(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->GetItemDataRowName()))
	{
		ProductItemSocketName = EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetMainEquipmentSlotTag());
		ProductItemDurabilityPercent += EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->GetDurability() /
			EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->ItemData.ItemMaxDurability;
		ProductItemDurabilityPercent /= 2;
	}

	if (ProductItemData->ItemsToDestroy.Contains(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())->GetItemDataRowName()))
	{
		AItemBase* SecondaryItem = EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag());
		EquipmentComp->UnequipItem(SecondaryItem);
		SecondaryItem->Destroy();
	}
	if (ProductItemData->ItemsToDestroy.Contains(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->GetItemDataRowName()))
	{
		AItemBase* MainItem = EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag());
		EquipmentComp->UnequipItem(MainItem);
		MainItem->Destroy();
	}

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::OnCraftingFinished IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	ProductItem = World->SpawnActor<AItemBase>(ProductItemData->ItemActorClass, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(ProductItem))
	{
		ProductItem->SetDurability(ProductItemDurabilityPercent * ProductItem->ItemData.ItemMaxDurability);
		EquipmentComp->EquipItem(ProductItem);
		
	}

	Player->StopAnimMontage(ProductItemData->Animation);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

