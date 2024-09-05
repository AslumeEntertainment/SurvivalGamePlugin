// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_BasicAttackAbility.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/DamageDealingItem.h"
#include "Animation/SG_AnimInstance.h"
#include "GAS/SG_GameplayAbilityTargetData.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/DamageDealingComponent.h"
#include "AbilitySystemBlueprintLibrary.h"


void USGGA_BasicAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<ASG_Character>(ActorInfo->AvatarActor.Get());
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

	if (bCanBeMirrored && TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Left"))
	{
		AnimInst->bIsMirrored = true;
	}
	else if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Right"))
	{
		AnimInst->bIsMirrored = false;
	}

	TArray<UDamageDealingComponent*> PlayerDamageDealingComponents;  
	Player->GetComponents<UDamageDealingComponent>(PlayerDamageDealingComponents, true);

	if (bUsesItem)
	{
		AHumanCharacter* HumanPlayer = Cast<AHumanCharacter>(Player);
		if (!IsValid(HumanPlayer))
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility IsValid(HumanPlayer) == false"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		ADamageDealingItem* MainHandItem = Cast<ADamageDealingItem>(HumanPlayer->MainHandItem);
		ADamageDealingItem* SecondaryHandItem = Cast<ADamageDealingItem>(HumanPlayer->SecondaryHandItem);

		if (!IsValid(MainHandItem) && !IsValid(SecondaryHandItem))
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility IsValid(MainHandItem && SecondaryHandItem) == false"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		if (IsValid(MainHandItem))
		{
			PlayerDamageDealingComponents.Add(MainHandItem->GetDamageDealingComponent());
		}
		if (IsValid(SecondaryHandItem))
		{
			PlayerDamageDealingComponents.Add(SecondaryHandItem->GetDamageDealingComponent());
		}
	}
	
	FGameplayTagContainer FinalActiveDamageComponentTags = ActiveDamageComponentTags;
	if (AnimInst->bIsMirrored)
	{
		FinalActiveDamageComponentTags = GetMirroredAbilityDamageComponentTags();
	}

	for (UDamageDealingComponent* DamageDealingComponent : PlayerDamageDealingComponents)
	{
		if (FinalActiveDamageComponentTags.HasTag(DamageDealingComponent->GetDamageComponentTag()))
		{
			Player->ActiveDamageDealingComponents.Add(DamageDealingComponent);
			for (const FDamageGameplayEffect DamageEffect : AdditionalDamageEffects)
			{
				DamageDealingComponent->ConfigDamageGameplayEffect(DamageEffect);
			}
		}
	}
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackAnimationMontage);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageBlendOut);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();
}

FGameplayTagContainer USGGA_BasicAttackAbility::GetMirroredAbilityDamageComponentTags()
{
	FGameplayTagContainer MirroredTags;
	for (FGameplayTag DamageComponentTag : ActiveDamageComponentTags)
	{
		FString TagName = DamageComponentTag.ToString();
		if (TagName.Contains(".Left"))
		{
			TagName = DamageComponentTag.RequestDirectParent().ToString();
			TagName = TagName + TEXT(".Right");
		}
		else if (TagName.Contains(".Right"))
		{
			TagName = DamageComponentTag.RequestDirectParent().ToString();
			TagName = TagName + TEXT(".Left");
		}
		MirroredTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TagName), true));
		UE_LOG(LogTemp, Warning, TEXT("New Ability GameplayTag - %s"), *TagName);
	}

	return MirroredTags;
}

void USGGA_BasicAttackAbility::OnAnimMontageCompleted()
{
	if (AnimInst->bIsMirrored)
	{
		AnimInst->bIsMirrored = false;
	}
	for (UDamageDealingComponent* DamageDealingComponent : Player->ActiveDamageDealingComponents)
	{
		DamageDealingComponent->ResetDamageEffects();
	}
	Player->ActiveDamageDealingComponents.Empty();
	
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability complete!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_BasicAttackAbility::OnAnimMontageBlendOut()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Attack over, blending out anim montage..."));
}

void USGGA_BasicAttackAbility::OnAnimMontageCanceled()
{
	if (AnimInst->bIsMirrored)
	{
		AnimInst->bIsMirrored = false;
	}
	for (UDamageDealingComponent* DamageDealingComponent : Player->ActiveDamageDealingComponents)
	{
		DamageDealingComponent->ResetDamageEffects();
	}
	Player->ActiveDamageDealingComponents.Empty();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
