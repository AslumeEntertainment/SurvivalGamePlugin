// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_BasicAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Interactables/Items/DamageDealingItem.h"
#include "Components/DamageDealingComponent.h"
#include "Components/EquipmentComponent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_GameplayAbilityTargetData.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Animation/SG_AnimInstance.h"
#include "Characters/InGameCharacter.h"


void USGGA_BasicAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UEquipmentComponent* EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Player->SetIsMirrored(bShouldMirrorAnimation);
	Player->SetMovementEnabled(bCanMoveDuringAnimation);

	float AttackSpeed = Player->GetAttackSpeed();

	if (DoesAnimationMatchSkeleton(Player->GetMesh()))
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AnimationMontage, AttackSpeed);
		PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCompleted);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageBlendOut);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageInterrupted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCanceled);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Ability animation does not match avatar's skeleton"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USGGA_BasicAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Player->SetIsMirrored(false);
	Player->SetMovementEnabled(true);
	Player->SetActivenessForAllDamageSources(false);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

