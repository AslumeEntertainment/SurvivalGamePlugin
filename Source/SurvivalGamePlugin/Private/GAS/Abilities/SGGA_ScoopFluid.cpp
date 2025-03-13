// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ScoopFluid.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Interactables/Items/FluidContainerItem.h"
#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"


void USGGA_ScoopFluid::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ScoopFluid::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ScoopFluid::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	UEquipmentComponent* EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ScoopFluid::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (EquipmentComp->HasEquipmentSlotWithTag(EquipmentComp->GetMixedEquipmentSlotTag()))
	{
		FluidContainerItem = Cast<AFluidContainerItem>(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMixedEquipmentSlotTag()));
	}
	else
	{
		FluidContainerItem = Cast<AFluidContainerItem>(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag()));
	}

	if (IsValid(FluidContainerItem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ScoopFluid::ActivateAbility EffectGrantingItem == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	Player->SetIsMirrored(bShouldMirrorAnimation);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AnimationMontage);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_ScoopFluid::OnAnimMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USGGA_ScoopFluid::OnAnimMontageInterrupted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_ScoopFluid::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OnNotifyRecievedGameplayEventTag, nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_ScoopFluid::OnEventRecieved);
	WaitGameplayEventTask->ReadyForActivation();
}

void USGGA_ScoopFluid::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Player->SetIsMirrored(false);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_ScoopFluid::OnEventRecieved(FGameplayEventData Payload)
{
	FluidContainerItem->Scoop();
}
