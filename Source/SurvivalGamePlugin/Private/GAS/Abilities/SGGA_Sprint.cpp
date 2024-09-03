// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_Sprint.h"
#include "Characters/SG_Character.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"


void USGGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	float NewSpeed = ActorInfo->AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute()) * SpeedMultiplier;
	ActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute(), NewSpeed);
	ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsRunning")));

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_Sprint::OnSprintButtonReleased);
	WaitInputReleaseTask->Activate();
}

void USGGA_Sprint::OnSprintButtonReleased(float TimeHeld)
{
	float OldSpeed = CurrentActorInfo->AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute()) / SpeedMultiplier;
	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute(), OldSpeed);
	CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsRunning")));
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
