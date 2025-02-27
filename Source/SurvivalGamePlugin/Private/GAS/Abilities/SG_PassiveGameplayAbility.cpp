// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SG_PassiveGameplayAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"


void USG_PassiveGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_PassiveGameplayAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (bShouldTriggerOnHitReceived)
	{
		OnHitReceivedEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Received"), nullptr, false, true);
		OnHitReceivedEventTask->EventReceived.AddDynamic(this, &USG_PassiveGameplayAbility::OnHitEventReceived);
		OnHitReceivedEventTask->ReadyForActivation();
	}
	if (bShouldTriggerOnHitDealt)
	{
		OnHitTriggeredEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Triggered"), nullptr, false, true);
		OnHitTriggeredEventTask->EventReceived.AddDynamic(this, &USG_PassiveGameplayAbility::OnHitEventDealt);
		OnHitTriggeredEventTask->ReadyForActivation();
	}
}

void USG_PassiveGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	OnHitReceivedEventTask->EndTask();
	OnHitTriggeredEventTask->EndTask();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
