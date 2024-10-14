// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_Sprint.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/SG_Character.h"


void USGGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	FGameplayEffectSpecHandle EffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(SpeedIncreaseEffect, 1, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();
	EffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("GameplayEffectCaller.Speed"), SpeedMultiplier);
	SpeedIncreaseEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec);

	ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsRunning")));

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_Sprint::OnSprintButtonReleased);
	WaitInputReleaseTask->ReadyForActivation();
}

void USGGA_Sprint::OnSprintButtonReleased(float TimeHeld)
{
	CurrentActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(SpeedIncreaseEffectSpecHandle);
	CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsRunning")));
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
