// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ApplyEffectToSelf.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

#include "GAS/SG_AbilitySystemComponent.h"

void USGGA_ApplyEffectToSelf::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectToSelf::ActivateAbility CommitAbility() == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	UAbilityTask_WaitGameplayEvent* WaitMixedGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag, nullptr, false, true);
	WaitMixedGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_ApplyEffectToSelf::OnEventReceived);
	WaitMixedGameplayEventTask->ReadyForActivation();
}

void USGGA_ApplyEffectToSelf::OnEventReceived(FGameplayEventData Payload)
{
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectToSelf::OnEventReceived IsValid(ASC) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	ASC->AddEffectFromClass(EffectClass, Payload.Instigator);
}
