// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_BasicPunch.h"
#include "Characters/HumanCharacter.h"
#include "Animation/SG_AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


void USGGA_BasicPunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicPunch::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AHumanCharacter* Player = CastChecked<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicPunch::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AnimInst = Cast<USG_AnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicPunch::ActivateAbility IsValid(AnimInst) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
	if (TriggerEventData->EventTag.GetTagName() == FName("Punch.Left"))
	{
		AnimInst->bIsMirrored = true;
	}
	else if(TriggerEventData->EventTag.GetTagName() == FName("Punch.Right"))
	{
		AnimInst->bIsMirrored = false;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AnimationMontage);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USGGA_BasicPunch::OnMontageBlendOut);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_BasicPunch::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_BasicPunch::OnMontageBlendOut);
	PlayMontageTask->ReadyForActivation();
}

void USGGA_BasicPunch::OnMontageBlendOut()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("PUNCH END!"));
}

void USGGA_BasicPunch::OnMontageCompleted()
{
	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
