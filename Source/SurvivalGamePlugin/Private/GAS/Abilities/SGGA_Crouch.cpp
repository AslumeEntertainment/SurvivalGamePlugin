// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_Crouch.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/SG_Character.h"


void USGGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_Crouch::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Player = Cast<ASG_Character>(ActorInfo->AvatarActor);
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_Crouch::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Player->Crouch();

	FGameplayEffectSpecHandle SpeedEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(SpeedDecreaseEffect, 1, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	FGameplayEffectSpec* SpeedEffectSpec = SpeedEffectSpecHandle.Data.Get();
	SpeedEffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("GameplayEffectCaller.Speed"), SpeedMultiplier);
	SpeedDecreaseEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpeedEffectSpec);

	FGameplayEffectSpecHandle JumpVelocityEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(JumpVelocityDecreaseEffect, 1, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	FGameplayEffectSpec* JumpVelocityEffectSpec = JumpVelocityEffectSpecHandle.Data.Get();
	JumpVelocityEffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("GameplayEffectCaller.JumpVelocity"), JumpVelocityMultiplier);
	JumpVelocityDecreaseEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*JumpVelocityEffectSpec);

	ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsCrouching")));

	UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPressTask->OnPress.AddDynamic(this, &USGGA_Crouch::OnCrouchPressedAgain);
	WaitInputPressTask->ReadyForActivation();
}

void USGGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(SpeedDecreaseEffectSpecHandle);
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(JumpVelocityDecreaseEffectSpecHandle);
	ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.IsCrouching")));

	if (IsValid(Player) && Player->bIsCrouched)
	{
		Player->UnCrouch();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_Crouch::OnCrouchPressedAgain(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
