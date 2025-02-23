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
		return;
	}

	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent);
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectToSelf::ActivateAbility IsValid(ASC) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	const AActor* Instigator = (TriggerEventData == nullptr) ? nullptr : TriggerEventData->Instigator;

	ASC->AddEffectFromClass(EffectClass, Instigator, nullptr, nullptr);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

