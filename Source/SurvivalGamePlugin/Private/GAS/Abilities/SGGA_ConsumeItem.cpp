// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ConsumeItem.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Objects/ConsumableItemEffectDataObject.h"
#include "Interactables/Items/ConsumableItem.h"
#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "Animation/SG_AnimInstance.h"

void USGGA_ConsumeItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ConsumeItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ConsumeItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	UEquipmentComponent* EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ConsumeItem::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (EquipmentComp->HasEquipmentSlotWithTag(EquipmentComp->GetMixedEquipmentSlotTag()))
	{
		ConsumableItem = Cast<AConsumableItem>(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag()));
	}
	else
	{
		ConsumableItem = Cast<AConsumableItem>(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag()));
	}
	if (IsValid(ConsumableItem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ConsumeItem::ActivateAbility EffectGrantingItem == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Left"))
	{
		Player->SetIsMirrored(true);
	}
	else if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Right"))
	{
		Player->SetIsMirrored(false);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UseAnimationMontage);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_ConsumeItem::OnAnimMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USGGA_ConsumeItem::OnAnimMontageInterrupted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_ConsumeItem::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OnNotifyRecievedGameplayEventTag, nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_ConsumeItem::OnEventRecieved);
	WaitGameplayEventTask->ReadyForActivation();
}

void USGGA_ConsumeItem::OnAnimMontageCompleted()
{
	Player->SetIsMirrored(false);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_ConsumeItem::OnAnimMontageInterrupted()
{
	Player->SetIsMirrored(false);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("USGGA_ConsumeItem: Montage interrupted!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_ConsumeItem::OnAnimMontageCanceled()
{
	Player->SetIsMirrored(false);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("USGGA_ConsumeItem: Ability canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_ConsumeItem::OnEventRecieved(FGameplayEventData Payload)
{
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::OnEventRecieved IsValid(ASC) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	for (UConsumableItemEffectDataObject* EffectDataObject : ConsumableItem->GetEffects())
	{
		if (EffectDataObject == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::OnEventRecieved EffectDataObject == nullptr"));
			continue;
		}
		ASC->AddEffectFromClass(EffectDataObject->EffectClass, EffectDataObject, EffectDataObject->EffectLevel);
	}

	ConsumableItem->LoseDurability(1.f);
}
