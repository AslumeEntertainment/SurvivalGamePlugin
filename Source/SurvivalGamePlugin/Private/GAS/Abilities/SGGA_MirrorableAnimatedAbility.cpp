// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_MirrorableAnimatedAbility.h"

#include "Components/SkeletalMeshComponent.h"


void USGGA_MirrorableAnimatedAbility::OnAnimMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageBlendOut()
{
	if (bWaitForAnimationToComplete)
	{
		UWorld* World = GetWorld();
		if (IsValid(World) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_MirrorableAnimatedAbility::OnAnimMontageBlendOut IsValid(World) == false"));
			return;
		}
		
		World->GetTimerManager().SetTimer(EmergencyAbilityEndTimer, this, 
			&USGGA_MirrorableAnimatedAbility::OnAnimMontageCompleted, 1.f, false);

		return;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageCanceled()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_MirrorableAnimatedAbility::OnAnimMontageCanceled IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (World->GetTimerManager().IsTimerActive(EmergencyAbilityEndTimer))
	{
		World->GetTimerManager().ClearTimer(EmergencyAbilityEndTimer);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

bool USGGA_MirrorableAnimatedAbility::DoesAnimationMatchSkeleton(USkeletalMeshComponent* SkeletalMesh)
{
	if (IsValid(AnimationMontage) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_MirrorableAnimatedAbility::DoesAnimationMatchSkeleton IsValid(AnimationMontage) == false"));
		return false;
	}
	if (IsValid(SkeletalMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_MirrorableAnimatedAbility::DoesAnimationMatchSkeleton IsValid(SkeletalMesh) == false"));
		return false;
	}

	return AnimationMontage->GetSkeleton() == SkeletalMesh->SkeletalMesh->Skeleton;
}
