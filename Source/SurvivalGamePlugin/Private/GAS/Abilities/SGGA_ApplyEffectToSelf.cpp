// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ApplyEffectToSelf.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

#include "Objects/GameplayEffectDataObjects/GameplayEffectData.h"
#include "GAS/SG_AbilitySystemComponent.h"


void USGGA_ApplyEffectToSelf::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectToSelf::ActivateAbility CommitAbility() == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (IsValid(EffectData) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectToSelf::ActivateAbility IsValid(EffectData) == false"));
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

	//na slaid 3/7 po IIKT e GHz, a ne MHz

	if (TriggerEventData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectToSelf::ActivateAbility TriggerEventData == nullptr"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectData->EffectClass, EffectData->EffectLevel, TriggerEventData->ContextHandle);
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectToSelf::ActivateAbility Spec == nullptr"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASC->ApplyGameplayEffectSpecToSelf(*Spec);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

