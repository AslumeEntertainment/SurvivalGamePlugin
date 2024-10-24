// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_CraftItem.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Components/EquipmentComponent.h"
#include "UI/Widgets/CraftingWindowWidget.h"
#include "UI/Widgets/ItemIconWidget.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"
#include "Animation/SG_AnimInstance.h"


bool USGGA_CraftItem::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
	AInGameCharacter* PlayerRef = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(PlayerRef) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::CanActivateAbility IsValid(HumanPlayer) == false"));
		return false;
	}
	UEquipmentComponent* EquipmentComponent = PlayerRef->GetEquipmentComponent();
	if (IsValid(EquipmentComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UnequipItem::ActivateAbility IsValid(EquipmentComp) == false"));
		return false;
	}
	if (IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMainEquipmentSlotTag())) == false || 
		IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetSecondaryEquipmentSlotTag())) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem: Player needs to have an item in both hands!"));
		return false;
	}
	if (EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMainEquipmentSlotTag())->ItemData.bCanCraftWithItem(EquipmentComponent->
		GetEquipedItemAtSlot(EquipmentComponent->GetSecondaryEquipmentSlotTag())->GetItemDataRowName()) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem: Player cannot craft anything with their current items!"));
		return false;
	}

	return true;
}

void USGGA_CraftItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	OpenCraftingWindow();

	WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_CraftItem::CancelCrafting);
	WaitInputReleaseTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitStartCraftingTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Craft"), nullptr, true, false);
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

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(World) == false"));
		return;
	}

	CraftingWindow = CreateWidget<UCraftingWindowWidget>(UGameplayStatics::GetGameInstance(World), CraftingWindowWidgetClass);

	PlayerCont = Cast<APlayerController>(Player->GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::OpenCraftingWindow IsValid(PlayerCont) == false"));
		return;
	}

	for (FItemCraftingDataStruct CraftingRecipe : EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->ItemData.CraftableItems)
	{
		if (CraftingRecipe.IngredientItemID == EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())->GetItemDataRowName())
		{
			UItemIconWidget* ItemIcon = CreateWidget<UItemIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), ItemIconWidgetClass);
			ItemIcon->SetItemImage(CraftingRecipe.ProductItemImage);
			ItemIcon->SetItemName(CraftingRecipe.ProductItemName);
			ItemIcon->IconClicked.AddDynamic(this, &USGGA_CraftItem::OnItemToCraftSelected);
			CraftingWindow->AddCraftingRecipe(ItemIcon);
		}
	}

	CraftingWindow->AddToViewport();
	PlayerCont->SetInputMode(FInputModeGameAndUI());
	PlayerCont->bShowMouseCursor = true;
}

void USGGA_CraftItem::CloseCraftingWindow()
{
	if (IsLocallyControlled() == false)
	{
		return;
	}

	CraftingWindow->RemoveFromParent();
	CraftingWindow->ClearRecipes();

	PlayerCont->SetInputMode(FInputModeGameOnly());
	PlayerCont->bShowMouseCursor = false;
}

void USGGA_CraftItem::CancelCrafting(float TimeHeld)
{
	CloseCraftingWindow();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_CraftItem::OnItemToCraftSelected(FText itemName)
{
	CloseCraftingWindow();

	FString GameplayTagString = FString(TEXT("Craft.")) + itemName.ToString();
	GameplayTagString.RemoveSpacesInline();
	FGameplayTag ProductNameTag = FGameplayTag::RequestGameplayTag(FName(GameplayTagString));

	Player->SendGameplayEventToSelf(ProductNameTag, FGameplayEventData());
}

void USGGA_CraftItem::StartCrafting(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem: Starting to craft!"));

	WaitInputReleaseTask->EndTask();

	if (Player->IsRightHanded() == false)
	{
		Player->SetIsMirrored(true);
	}
	if (!IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())) || !IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::StartCrafting IsValid(Player->MainHandItem || Player->SecondaryHandItem) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	FName ProductItemName = FName(Payload.EventTag.ToString().RightChop(6)); //Removing the "Craft." prefix

	ProductItemData = CraftingInfoTable->FindRow<FCraftingProcessDataStruct>(ProductItemName, "");
	if (ProductItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility ProductItemData == nullptr"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	SpawnProductItem();

	FGameplayEffectSpecHandle EffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(SpeedReductionEffect, 1, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();
	EffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("GameplayEffectCaller.Speed"), ProductItemData->MovementSpeedMultiplier);
	SpeedReductionEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ProductItemData->Animation);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_CraftItem::OnAnimMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_CraftItem::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitFinishCraftingTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Notify.FinishCrafting"), nullptr, true, true);
	WaitFinishCraftingTask->EventReceived.AddDynamic(this, &USGGA_CraftItem::OnCraftingFinished);
	WaitFinishCraftingTask->ReadyForActivation();
}

void USGGA_CraftItem::SpawnProductItem()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::OnCraftingFinished IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	
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

	ProductItem = World->SpawnActor<AItemBase>(ProductItemData->ItemActorClass, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(ProductItem))
	{
		ProductItem->SetActorScale3D(FVector(0.0001, 0.0001, 0.0001));

		ProductItem->SetDurability(ProductItemDurabilityPercent * ProductItem->ItemData.ItemMaxDurability);
	}

	UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem::SpawnProductItem - Product item has been spawned and kept hidden until ability has finished"));
}

void USGGA_CraftItem::OnAnimMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Crafting: Montage has been completed"));
	
	Player->SetIsMirrored(false);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_CraftItem::OnAnimMontageCanceled()
{
	UE_LOG(LogTemp, Warning, TEXT("Crafting: Montage has been canceled"));

	Player->SetIsMirrored(false);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_CraftItem::OnCraftingFinished(FGameplayEventData Payload)
{
	if (ProductItemData->ItemsToDestroy.Contains(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())->GetItemDataRowName()))
	{
		EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())->DestroyItem();
	}
	if (ProductItemData->ItemsToDestroy.Contains(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->GetItemDataRowName()))
	{
		EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->DestroyItem();
	}

	if (IsValid(ProductItem)) 
	{
		if (ProductItemSocketName == EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetMainEquipmentSlotTag()))
		{
			EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag(), ProductItem);
		}
		else if (ProductItemSocketName == EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetSecondaryEquipmentSlotTag()))
		{
			EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag(), ProductItem);
		}
		ProductItem->OnItemEquiped(Player, ProductItemSocketName);
	}

	UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem::OnCraftingFinished - Crafting process finished!"));
}

