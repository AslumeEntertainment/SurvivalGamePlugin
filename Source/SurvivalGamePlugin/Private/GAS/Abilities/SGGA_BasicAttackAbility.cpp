// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_BasicAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Interactables/Items/DamageDealingItem.h"
#include "Components/DamageDealingComponent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_GameplayAbilityTargetData.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Animation/SG_AnimInstance.h"
#include "Characters/HumanCharacter.h"


void USGGA_BasicAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
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
		Player->SetIsMirrored(true);
	}
	else if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Right"))
	{
		Player->SetIsMirrored(false);
	}

	/* Getting every damage dealing component that the user has */
	TArray<UDamageDealingComponent*> PlayerDamageDealingComponents;  
	Player->GetComponents<UDamageDealingComponent>(PlayerDamageDealingComponents, true);

	/* Look at all Gameplay effects that are currently active on the user's ASC and determine if there are additional effect to be applied */
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent);
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicAttackAbility::ActivateAbility IsValid(ASC) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
	TArray<FDamageGameplayEffect> InstigatedEffects;
	TArray<FGameplayEffectSpec> ActiveGameplayEffectSpecs;
	ASC->GetAllActiveGameplayEffectSpecs(ActiveGameplayEffectSpecs);

	for (FGameplayEffectSpec ActiveSpec : ActiveGameplayEffectSpecs)
	{
		TSubclassOf<UGameplayEffect>* GameEffectPtr = DamagingEffectPerPlayerEffectTable.Find(ActiveSpec.Def.GetClass());
		if (GameEffectPtr == nullptr)
		{
			continue;
		}

		FDamageGameplayEffect InstigatedEffect;
		InstigatedEffect.DamageEffectClass = *GameEffectPtr;
		InstigatedEffect.SetByCallerMagnitudeDataTag = FGameplayTag::RequestGameplayTag("GameplayEffectCaller.Damage");
		InstigatedEffect.DamageValue = -ActiveSpec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("GameplayEffectCaller.Damage"), false);

		InstigatedEffects.Add(InstigatedEffect);
	}

	/* Does the ability use an item */
	if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains("Item"))
	{
		TArray<ADamageDealingItem*> EquipedWeapons = Player->GetAllEquipedWeapons();
		for (ADamageDealingItem* Weapon : EquipedWeapons)
		{
			PlayerDamageDealingComponents.Add(Weapon->GetDamageDealingComponent());
		}
	}
	
	/* Mirroring the tags from the list of damage dealing components that are to be used for this ability */
	FGameplayTagContainer FinalActiveDamageComponentTags = ActiveDamageComponentTags;
	if (Player->IsMirrored())
	{
		FinalActiveDamageComponentTags = GetMirroredAbilityDamageComponentTags();
	}

	/* Config Gameplay effects that are to be used by the active damage dealing components */
	for (UDamageDealingComponent* DamageDealingComponent : PlayerDamageDealingComponents)
	{
		if (FinalActiveDamageComponentTags.HasTag(DamageDealingComponent->GetDamageComponentTag()))
		{
			Player->ActiveDamageDealingComponents.Add(DamageDealingComponent);
			for (const FDamageGameplayEffect DamageEffect : AdditionalDamageEffects)
			{
				DamageDealingComponent->ConfigDamageGameplayEffect(DamageEffect);
			}
			for (const FDamageGameplayEffect DamageEffect : InstigatedEffects)
			{
				DamageDealingComponent->ConfigDamageGameplayEffect(DamageEffect);
			}
		}
	}

	float AttackSpeed = Player->GetAttackSpeed();

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackAnimationMontage, AttackSpeed);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageBlendOut);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_BasicAttackAbility::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();
}

void USGGA_BasicAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	for (UDamageDealingComponent* DamageDealingComponent : Player->ActiveDamageDealingComponents)
	{
		if (IsValid(DamageDealingComponent))
		{
			DamageDealingComponent->ResetDamageEffects();
		}
	}
	Player->ActiveDamageDealingComponents.Empty();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
	}

	return MirroredTags;
}

void USGGA_BasicAttackAbility::OnAnimMontageCompleted()
{
	if (Player->IsMirrored())
	{
		Player->SetIsMirrored(false);
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Montage complete!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_BasicAttackAbility::OnAnimMontageBlendOut()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Montage blending out..."));
}

void USGGA_BasicAttackAbility::OnAnimMontageCanceled()
{
	if (Player->IsMirrored())
	{
		Player->SetIsMirrored(false);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Montage canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
