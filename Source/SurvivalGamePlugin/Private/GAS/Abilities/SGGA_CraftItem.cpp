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
#include "UI/Widgets/CraftingWindowWidget.h"
#include "UI/Widgets/ItemIconWidget.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/HumanCharacter.h"
#include "Animation/SG_AnimInstance.h"


void USGGA_CraftItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(USGGA_CraftItem, ProductItem);
	//DOREPLIFETIME(USGGA_CraftItem, ProductItemSocketName);
}

bool USGGA_CraftItem::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
	AHumanCharacter* HumanPlayer = Cast<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(HumanPlayer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::CanActivateAbility IsValid(HumanPlayer) == false"));
		return false;
	}
	
	if (IsValid(HumanPlayer->GetEquipedItemAtSlot(EEquipmentSlot::Main)) == false || IsValid(HumanPlayer->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem: Player needs to have an item in both hands!"));
		return false;
	}

	if (HumanPlayer->GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.bCanCraftWithItem(HumanPlayer->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName()) == false)
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
	Player = Cast<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	OldSpeed = ActorInfo->AbilitySystemComponent->GetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute());

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
	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute(), OldSpeed);
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

	for (FItemCraftingDataStruct CraftingRecipe : Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.CraftableItems)
	{
		if (CraftingRecipe.IngredientItemID == Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName())
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

	AnimInst = Cast<USG_AnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::StartCrafting IsValid(AnimInst) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	if (Player->bIsRightHanded == false)
	{
		AnimInst->bIsMirrored = true;
	}
	if (!IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)) || !IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
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

	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute(), OldSpeed * ProductItemData->MovementSpeedMultiplier);

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

	if (ProductItemData->ItemsToDestroy.Contains(Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName()))
	{
		ProductItemSocketName = Player->GetEquipmentSocketOfType(EEquipmentSlot::Secondary);
		ProductItemDurabilityPercent += Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetDurability() / Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->ItemData.ItemMaxDurability;
	}
	if (ProductItemData->ItemsToDestroy.Contains(Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->GetItemDataRowName()))
	{
		ProductItemSocketName = Player->GetEquipmentSocketOfType(EEquipmentSlot::Main);
		ProductItemDurabilityPercent += Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->GetDurability() / Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.ItemMaxDurability;
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
	
	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_CraftItem::OnAnimMontageCanceled()
{
	UE_LOG(LogTemp, Warning, TEXT("Crafting: Montage has been canceled"));

	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_CraftItem::OnCraftingFinished(FGameplayEventData Payload)
{
	if (ProductItemData->ItemsToDestroy.Contains(Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName()))
	{
		Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->DestroyItem();
	}
	if (ProductItemData->ItemsToDestroy.Contains(Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->GetItemDataRowName()))
	{
		Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->DestroyItem();
	}

	if (IsValid(ProductItem)) 
	{
		if (ProductItemSocketName == Player->GetEquipmentSocketOfType(EEquipmentSlot::Main))
		{
			Player->SetEquipedItemAtSlot(EEquipmentSlot::Main, ProductItem);
		}
		else if (ProductItemSocketName == Player->GetEquipmentSocketOfType(EEquipmentSlot::Secondary))
		{
			Player->SetEquipedItemAtSlot(EEquipmentSlot::Secondary, ProductItem);
		}
		ProductItem->OnItemEquiped(Player, ProductItemSocketName);
	}

	UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem::OnCraftingFinished - Crafting process finished!"));
}

