// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_BasicAttackAbility.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"
#include "Headers/SG_GameplayAbilityTargetData.h"
#include "Animation/SG_AnimInstance.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void USGGA_BasicAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<ASG_Character>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AnimInst = Cast<USG_AnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility IsValid(AnimInst) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Left"))
	{
		AnimInst->bIsMirrored = true;
	}
	else if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Right"))
	{
		AnimInst->bIsMirrored = false;
	}

	if (Cast<AHumanCharacter>(Player) && bUsesItem && TriggerEventData->TargetData.Data[0].IsValid())
	{
		FSG_GameplayAbilityTargetData* CustomTargetData = static_cast<FSG_GameplayAbilityTargetData*>(TriggerEventData->TargetData.Data[0].Get());
		if (CustomTargetData == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CustomTargetData == nullptr"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		Cast<AHumanCharacter>(Player)->ActiveItem = CustomTargetData->ItemActor;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackAnimationMontage);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageBlendOut);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OnHitGameplayEventTag, nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_BasicAttackAbility::OnEventRecieved);
	WaitGameplayEventTask->Activate();
}

void USGGA_BasicAttackAbility::OnAnimMontageCompleted()
{
	AnimInst->bIsMirrored = false;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability complete!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_BasicAttackAbility::OnAnimMontageBlendOut()
{
	if (Cast<AHumanCharacter>(Player))
	{
		Cast<AHumanCharacter>(Player)->ActiveItem = nullptr;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Attack over, blending out anim montage..."));
}

void USGGA_BasicAttackAbility::OnAnimMontageCanceled()
{
	AnimInst->bIsMirrored = false;
	if (Cast<AHumanCharacter>(Player))
	{
		Cast<AHumanCharacter>(Player)->ActiveItem = nullptr;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_BasicAttackAbility::OnEventRecieved(FGameplayEventData Payload)
{
	if (Cast<AHumanCharacter>(Player) != nullptr && IsValid(Cast<AHumanCharacter>(Player)->ActiveItem))
	{
		Cast<AHumanCharacter>(Player)->ActiveItem->ServerLoseDurability(20.f);
	}
	ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, Payload.TargetData, GameplayEffectClass, 1, 1);
}
