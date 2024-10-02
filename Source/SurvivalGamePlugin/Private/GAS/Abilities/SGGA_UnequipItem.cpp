// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_UnequipItem.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/HumanCharacter.h"


void USGGA_UnequipItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UnequipItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = CastChecked<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UnequipItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsUnequiping")));

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_UnequipItem::OnUnequipCanceled);
	WaitInputReleaseTask->ReadyForActivation();

	UE_LOG(LogTemp, Warning, TEXT("USGGA_UnequipItem::ActivateAbility Unequip start!"));

	if (IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)))
	{
		UAbilityTask_WaitGameplayEvent* WaitMainGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("PlayerAction.UnequipMain"), nullptr, true, true);
		WaitMainGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_UnequipItem::UnequipMain);
		WaitMainGameplayEventTask->ReadyForActivation();
		UE_LOG(LogTemp, Warning, TEXT("USGGA_UnequipItem::ActivateAbility Waiting to confirm main item drop..."));
	}
	if (IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
	{
		UAbilityTask_WaitGameplayEvent* WaitSecondaryGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("PlayerAction.UnequipSecondary"), nullptr, true, true);
		WaitSecondaryGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_UnequipItem::UnequipSecondary);
		WaitSecondaryGameplayEventTask->ReadyForActivation();
		UE_LOG(LogTemp, Warning, TEXT("USGGA_UnequipItem::ActivateAbility Waiting to confirm secondary item drop..."));
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
	Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->OnItemUnequiped();
	Player->SetEquipedItemAtSlot(EEquipmentSlot::Main, nullptr);

	UE_LOG(LogTemp, Warning, TEXT("USGGA_UnequipItem::ActivateAbility Dropping main item confirmed!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


void USGGA_UnequipItem::UnequipSecondary(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}
	Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->OnItemUnequiped();
	Player->SetEquipedItemAtSlot(EEquipmentSlot::Secondary, nullptr);

	UE_LOG(LogTemp, Warning, TEXT("USGGA_UnequipItem::ActivateAbility Dropping secondary item confirmed!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_UnequipItem::OnUnequipCanceled(float TimeHeld)
{
	UE_LOG(LogTemp, Warning, TEXT("USGGA_UnequipItem::ActivateAbility Unequip canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
