// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_Sprint.h"
#include "Characters/SG_Character.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"


void USGGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	EffectHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, SprintGameplayEffectClass.GetDefaultObject(), 1, 1);

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_Sprint::OnSprintButtonReleased);
	WaitInputReleaseTask->Activate();
}

void USGGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	BP_RemoveGameplayEffectFromOwnerWithHandle(EffectHandle);
}

void USGGA_Sprint::OnSprintButtonReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
