// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_OnEnergyDrained.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"

void USGGA_OnEnergyDrained::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_OnEnergyDrained::ActivateAbility CommitAbility() == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASC = ActorInfo->AbilitySystemComponent.Get();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_OnEnergyDrained::ActivateAbility IsValid(ASC) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASC->SetNumericAttributeBase(USG_CharacterAttributeSet::GetCurrentSpeedAttribute(), ASC->GetNumericAttributeBase(USG_CharacterAttributeSet::GetCurrentSpeedAttribute()) * SpeedMultiplier);

	UAbilityTask_WaitGameplayEvent* WaitMixedGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Bool.HasFull.Energy"), nullptr, true, true);
	WaitMixedGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_OnEnergyDrained::OnEnergyRecharged);
	WaitMixedGameplayEventTask->ReadyForActivation();
}

void USGGA_OnEnergyDrained::OnEnergyRecharged(FGameplayEventData Payload)
{
	ASC->SetNumericAttributeBase(USG_CharacterAttributeSet::GetCurrentSpeedAttribute(), ASC->GetNumericAttributeBase(USG_CharacterAttributeSet::GetCurrentSpeedAttribute()) / SpeedMultiplier);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}
