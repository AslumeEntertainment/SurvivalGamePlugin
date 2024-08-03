// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_BasicAttackAbility.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"
#include "Animation/SG_AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void USGGA_BasicAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AHumanCharacter* Player = CastChecked<AHumanCharacter>(ActorInfo->AvatarActor.Get());
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

	if (TriggerEventData->EventTag.ToString().Contains(".Left"))
	{
		AnimInst->bIsMirrored = true;
	}
	else if (TriggerEventData->EventTag.ToString().Contains(".Right"))
	{
		AnimInst->bIsMirrored = false;
	}

	/*if (bUsesItem)
	{
		UsedItem = Cast<AItemBase>(TriggerEventData->Target);
	}*/

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackAnimationMontage);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageBlendOut);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();
}

void USGGA_BasicAttackAbility::OnAnimMontageCompleted()
{
	AnimInst->bIsMirrored = false;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability complete!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_BasicAttackAbility::OnAnimMontageBlendOut()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Attack over, blending out anim montage..."));
}

void USGGA_BasicAttackAbility::OnAnimMontageCanceled()
{
	AnimInst->bIsMirrored = false;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
