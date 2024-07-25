// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_UseItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"
#include "Animation/AnimMontage.h"

void USGGA_UseItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UseItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AHumanCharacter* Player = CastChecked<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_UseItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (bIsUsingMainHand && IsValid(Player->GetMainHandItem()))
	{
		Player->GetMainHandItem()->Use();
	}
	else if (!bIsUsingMainHand && IsValid(Player->GetSecondaryHandItem()))
	{
		Player->GetSecondaryHandItem()->Use();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
