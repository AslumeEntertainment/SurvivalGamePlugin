// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_BasicRestoringAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

#include "Characters/InGameCharacter.h"
#include "Interactables/Items/RegeneratingItem.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Animation/SG_AnimInstance.h"


void USGGA_BasicRestoringAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	RegeneratingItem = Cast<ARegeneratingItem>(Player->GetEquipedItemAtSlot(Player->GetSecondaryEquipmentSlotTag()));
	if (IsValid(RegeneratingItem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility RegeneratingItem == nullptr"));
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

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackAnimationMontage);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_BasicRestoringAbility::OnAnimMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_BasicRestoringAbility::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OnNotifyRecievedGameplayEventTag, nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_BasicRestoringAbility::OnEventRecieved);
	WaitGameplayEventTask->ReadyForActivation();
}

void USGGA_BasicRestoringAbility::OnAnimMontageCompleted()
{
	Player->SetIsMirrored(false);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_BasicRestoringAbility::OnAnimMontageCanceled()
{
	Player->SetIsMirrored(false);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_BasicRestoringAbility::OnEventRecieved(FGameplayEventData Payload)
{
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::OnEventRecieved IsValid(ASC) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	
	ASC->AddEffectFromClass(GameplayEffectClass, RegeneratingItem);

	RegeneratingItem->LoseDurability(1.f);
}
