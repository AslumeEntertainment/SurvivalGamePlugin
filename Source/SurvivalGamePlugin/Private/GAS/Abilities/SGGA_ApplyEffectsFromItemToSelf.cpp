// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ApplyEffectsFromItemToSelf.h"
#include "Characters/SG_Character.h"
#include "Interactables/Items/EffectGrantingItem.h"
#include "GAS/SG_GameplayAbilityTargetData.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Animation/SG_AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"


void USGGA_ApplyEffectsFromItemToSelf::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<ASG_Character>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AnimInst = Cast<USG_AnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility IsValid(AnimInst) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (!TriggerEventData->TargetData.Data[0].IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility TriggerEventData->TargetData.Data[0].IsValid() == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	FSG_GameplayAbilityTargetData* CustomTargetData = static_cast<FSG_GameplayAbilityTargetData*>(TriggerEventData->TargetData.Data[0].Get());
	if (CustomTargetData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility CustomTargetData == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	EffectGrantingItem = Cast<AEffectGrantingItem>(CustomTargetData->ItemActor);
	if (!IsValid(EffectGrantingItem))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectsFromItemToSelf::ActivateAbility RegeneratingItem == nullptr"));
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

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UseAnimationMontage);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_ApplyEffectsFromItemToSelf::OnAnimMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_ApplyEffectsFromItemToSelf::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OnNotifyRecievedGameplayEventTag, nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_ApplyEffectsFromItemToSelf::OnEventRecieved);
	WaitGameplayEventTask->ReadyForActivation();
}

void USGGA_ApplyEffectsFromItemToSelf::OnAnimMontageCompleted()
{
	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_ApplyEffectsFromItemToSelf::OnAnimMontageCanceled()
{
	AnimInst->bIsMirrored = false;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_ApplyEffectsFromItemToSelf::OnEventRecieved(FGameplayEventData Payload)
{
	for (int i = 0; i < EffectGrantingItem->GetEffects().Num(); i++)
	{
		FGameplayEffectSpecHandle GrantedEffectSpecHandle = EffectGrantingItem->CreateEffectSpecHandle(i);
		Player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*GrantedEffectSpecHandle.Data.Get());
	}

	EffectGrantingItem->Server_LoseDurability(1.f);
}
