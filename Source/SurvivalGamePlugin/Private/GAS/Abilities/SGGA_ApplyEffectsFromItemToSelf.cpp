// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ApplyEffectsFromItemToSelf.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Interactables/Items/EffectGrantingItem.h"
#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "Animation/SG_AnimInstance.h"


void USGGA_ApplyEffectsFromItemToSelf::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	UEquipmentComponent* EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	EffectGrantingItem = Cast<AEffectGrantingItem>(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag()));
	if (IsValid(EffectGrantingItem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility EffectGrantingItem == nullptr"));
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
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_ApplyEffectsFromItemToSelf::OnAnimMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_ApplyEffectsFromItemToSelf::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OnNotifyRecievedGameplayEventTag, nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_ApplyEffectsFromItemToSelf::OnEventRecieved);
	WaitGameplayEventTask->ReadyForActivation();
}

void USGGA_ApplyEffectsFromItemToSelf::OnAnimMontageCompleted()
{
	Player->SetIsMirrored(false);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_ApplyEffectsFromItemToSelf::OnAnimMontageCanceled()
{
	Player->SetIsMirrored(false);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("USGGA_ApplyEffectsFromItemToSelf: Ability canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_ApplyEffectsFromItemToSelf::OnEventRecieved(FGameplayEventData Payload)
{
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::OnEventRecieved IsValid(ASC) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	TMap<TSubclassOf<UGameplayEffect>, uint8> EffectsToGive = EffectGrantingItem->GetEffects();
	for (TPair<TSubclassOf<UGameplayEffect>, int> Effect : EffectsToGive)
	{
		ASC->AddEffectFromClass(Effect.Key, Player, Effect.Value);
	}

	EffectGrantingItem->LoseDurability(1.f);
}
