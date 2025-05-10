// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SG_AnimInstance.h"

UAnimMontage* USG_AnimInstance::GetConsumeAnimMontage() const
{
	return ConsumeAnimMontage;
}

UAnimMontage* USG_AnimInstance::GetRoarAnimMontage() const
{
	return RoarAnimMontage;
}

UAnimMontage* USG_AnimInstance::GetDamageAnimMontage() const
{
	return DamageAnimMontage;
}
