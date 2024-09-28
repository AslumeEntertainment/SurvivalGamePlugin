// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_CraftItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"
#include "Animation/SG_AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"


void USGGA_CraftItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USGGA_CraftItem, ProductItem);
	DOREPLIFETIME(USGGA_CraftItem, ProductItemSocketName);
}

void USGGA_CraftItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = CastChecked<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	OldSpeed = ActorInfo->AbilitySystemComponent->GetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute());

	AnimInst = Cast<USG_AnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(AnimInst) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	if (!IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)) || !IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player->MainHandItem || Player->SecondaryHandItem) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	FName ProductItemName = FName(*TriggerEventData->EventTag.ToString().RightChop(6)); //Removing the "Craft." prefix
	ProductItemData = CraftingInfoTable->FindRow<FCraftingProcessDataStruct>(ProductItemName, "");
	if (ProductItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility ProductItemData == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (HasAuthority(&ActivationInfo))
	{
		SpawnProductItem();
	}

	ActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute(), OldSpeed * ProductItemData->MovementSpeedMultiplier);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ProductItemData->Animation);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_CraftItem::OnAnimMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_CraftItem::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Notify.FinishCrafting"), nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_CraftItem::OnCraftingFinished);
	WaitGameplayEventTask->ReadyForActivation();
}

void USGGA_CraftItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute(), OldSpeed);
	UE_LOG(LogTemp, Warning, TEXT("Crafting: EndAbility - Speed has been reset"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_CraftItem::SpawnProductItem()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::OnCraftingFinished IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	
	if (Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->GetItemDataRowName() == ProductItemData->ItemsToDestroy[0])
	{
		ProductItemSocketName = Player->GetEquipmentSocketOfType(EEquipmentSlot::Main);
	}
	else if (Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName() == ProductItemData->ItemsToDestroy[0])
	{
		ProductItemSocketName = Player->GetEquipmentSocketOfType(EEquipmentSlot::Secondary);
	}

	if (Player->bIsRightHanded == false)
	{
		AnimInst->bIsMirrored = true;
	}

	ProductItem = World->SpawnActor<AItemBase>(ProductItemData->ItemActorClass, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(ProductItem))
	{
		ProductItem->SetActorScale3D(FVector(0.0001, 0.0001, 0.0001));
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
	UE_LOG(LogTemp, Warning, TEXT("USGGA_CraftItem::OnCraftingFinished - Gameplay Event recieved"));

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

