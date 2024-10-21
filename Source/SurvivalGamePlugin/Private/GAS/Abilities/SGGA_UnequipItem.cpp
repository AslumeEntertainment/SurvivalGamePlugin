// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_UnequipItem.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"


void USGGA_UnequipItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UnequipItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UnequipItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsUnequiping")));

	if (Player->GetAllEquipedItems().Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("USGGA_UnequipItem::ActivateAbility No items to unequip!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_UnequipItem::OnUnequipCanceled);
	WaitInputReleaseTask->ReadyForActivation();

	if (IsValid(Player->GetEquipedItemAtSlot(Player->GetMainEquipmentSlotTag())) || IsValid(Player->GetEquipedItemAtSlot(Player->GetCombinedMainEquipmentSlotTag())))
	{
		UAbilityTask_WaitGameplayEvent* WaitMainGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Main"), nullptr, true, true);
		WaitMainGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_UnequipItem::UnequipMain);
		WaitMainGameplayEventTask->ReadyForActivation();
	}
	if (IsValid(Player->GetEquipedItemAtSlot(Player->GetSecondaryEquipmentSlotTag())) || IsValid(Player->GetEquipedItemAtSlot(Player->GetCombinedSecondaryEquipmentSlotTag())))
	{
		UAbilityTask_WaitGameplayEvent* WaitSecondaryGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Secondary"), nullptr, true, true);
		WaitSecondaryGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_UnequipItem::UnequipSecondary);
		WaitSecondaryGameplayEventTask->ReadyForActivation();
	}
	if (IsValid(Player->GetEquipedItemAtSlot(Player->GetMixedEquipmentSlotTag())) || IsValid(Player->GetEquipedItemAtSlot(Player->GetCombinedEquipmentSlotTag())))
	{
		UAbilityTask_WaitGameplayEvent* WaitMixedGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Mixed"), nullptr, true, true);
		WaitMixedGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_UnequipItem::UnequipMixed);
		WaitMixedGameplayEventTask->ReadyForActivation();
	}
}

void USGGA_UnequipItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsUnequiping")));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_UnequipItem::UnequipMain(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}
	
	if (IsValid(Player->GetEquipedItemAtSlot(Player->GetCombinedMainEquipmentSlotTag())))
	{
		Player->GetEquipedItemAtSlot(Player->GetCombinedMainEquipmentSlotTag())->OnItemUnequiped();
		Player->SetEquipedItemAtSlot(Player->GetCombinedMainEquipmentSlotTag(), nullptr);
		return;
	}
	
	Player->GetEquipedItemAtSlot(Player->GetMainEquipmentSlotTag())->OnItemUnequiped();
	Player->SetEquipedItemAtSlot(Player->GetMainEquipmentSlotTag(), nullptr);
}


void USGGA_UnequipItem::UnequipSecondary(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	if (IsValid(Player->GetEquipedItemAtSlot(Player->GetCombinedSecondaryEquipmentSlotTag())))
	{
		Player->GetEquipedItemAtSlot(Player->GetCombinedSecondaryEquipmentSlotTag())->OnItemUnequiped();
		Player->SetEquipedItemAtSlot(Player->GetCombinedSecondaryEquipmentSlotTag(), nullptr);
		return;
	}
	
	Player->GetEquipedItemAtSlot(Player->GetSecondaryEquipmentSlotTag())->OnItemUnequiped();
	Player->SetEquipedItemAtSlot(Player->GetSecondaryEquipmentSlotTag(), nullptr);
}

void USGGA_UnequipItem::UnequipMixed(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	if (IsValid(Player->GetEquipedItemAtSlot(Player->GetCombinedEquipmentSlotTag(), false)))
	{
		Player->GetEquipedItemAtSlot(Player->GetCombinedEquipmentSlotTag(), false)->OnItemUnequiped();
		Player->SetEquipedItemAtSlot(Player->GetCombinedEquipmentSlotTag(), nullptr, false);
		return;
	}

	Player->GetEquipedItemAtSlot(Player->GetMixedEquipmentSlotTag())->OnItemUnequiped();
	Player->SetEquipedItemAtSlot(Player->GetMixedEquipmentSlotTag(), nullptr);
}

void USGGA_UnequipItem::OnUnequipCanceled(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
