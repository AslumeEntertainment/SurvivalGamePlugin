// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SG_GameplayAbility.h"

#include "AbilitySystemComponent.h"

USG_GameplayAbility::USG_GameplayAbility()
{
}

void USG_GameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bShouldActivateOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
