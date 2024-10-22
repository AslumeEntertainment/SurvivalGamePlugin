// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_UnequipItem.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"


void USGGA_UnequipItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UnequipItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UnequipItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UnequipItem::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsUnequiping")));

	if (EquipmentComp->GetAllEquipedItems().Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("USGGA_UnequipItem::ActivateAbility No items to unequip!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_UnequipItem::OnUnequipCanceled);
	WaitInputReleaseTask->ReadyForActivation();

	if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())) || IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedMainEquipmentSlotTag())))
	{
		UAbilityTask_WaitGameplayEvent* WaitMainGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Main"), nullptr, true, true);
		WaitMainGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_UnequipItem::UnequipMain);
		WaitMainGameplayEventTask->ReadyForActivation();
	}
	if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())) || IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedSecondaryEquipmentSlotTag())))
	{
		UAbilityTask_WaitGameplayEvent* WaitSecondaryGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Secondary"), nullptr, true, true);
		WaitSecondaryGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_UnequipItem::UnequipSecondary);
		WaitSecondaryGameplayEventTask->ReadyForActivation();
	}
	if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag())) || IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedEquipmentSlotTag())))
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
	
	if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedMainEquipmentSlotTag())))
	{
		EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedMainEquipmentSlotTag())->OnItemUnequiped();
		EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetCombinedMainEquipmentSlotTag(), nullptr);
		return;
	}
	
	EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->OnItemUnequiped();
	EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag(), nullptr);
}


void USGGA_UnequipItem::UnequipSecondary(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedSecondaryEquipmentSlotTag())))
	{
		EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedSecondaryEquipmentSlotTag())->OnItemUnequiped();
		EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetCombinedSecondaryEquipmentSlotTag(), nullptr);
		return;
	}
	
	EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())->OnItemUnequiped();
	EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag(), nullptr);
}

void USGGA_UnequipItem::UnequipMixed(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedEquipmentSlotTag(), false)))
	{
		EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetCombinedEquipmentSlotTag(), false)->OnItemUnequiped();
		EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetCombinedEquipmentSlotTag(), nullptr, false);
		return;
	}

	EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag())->OnItemUnequiped();
	EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag(), nullptr);
}

void USGGA_UnequipItem::OnUnequipCanceled(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
