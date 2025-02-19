// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_MirrorableAnimatedAbility.h"

void USGGA_MirrorableAnimatedAbility::OnAnimMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageBlendOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_MirrorableAnimatedAbility::OnAnimMontageCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
