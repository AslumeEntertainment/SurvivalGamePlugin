// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_MirrorableAnimatedAbility.h"

#include "Components/SkeletalMeshComponent.h"


void USGGA_MirrorableAnimatedAbility::OnAnimMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageBlendOut()
{
	if (bWaitForAnimationToComplete == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

bool USGGA_MirrorableAnimatedAbility::DoesAnimationMatchSkeleton(USkeletalMeshComponent* SkeletalMesh)
{
	if (IsValid(SkeletalMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_MirrorableAnimatedAbility::DoesAnimationMatchSkeleton IsValid(SkeletalMesh) == false"));
		return false;
	}

	return AnimationMontage->GetSkeleton() == SkeletalMesh->SkeletalMesh->Skeleton;
}
