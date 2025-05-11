// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_AttemptDisassembleItem.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Net/UnrealNetwork.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "Components/EquipmentComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"
#include "Animation/SG_AnimInstance.h"


void USGGA_AttemptDisassembleItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AItemBase* ItemForDisassembly = EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag());
	if (IsValid(ItemForDisassembly) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility IsValid(ItemForDisassembly) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ItemForDisassemblyData = DisassemlyInfoTable->FindRow<FDisassemblyProcessDataStruct>(ItemForDisassembly->GetItemDataRowName(), "");
	if (ItemForDisassemblyData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility ItemForDisassemblyData == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	switch (ItemForDisassemblyData->DisassembleRequirement)
	{
		case EItemDisassembleRequirement::Unable:
		{
			UE_LOG(LogTemp, Warning, TEXT("Disassembling: Current item cannot be disassembled!"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		case EItemDisassembleRequirement::Always:
		{
			break;
		}
		case EItemDisassembleRequirement::EmptyHand:
		{
			if (IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())))
			{
				UE_LOG(LogTemp, Warning, TEXT("Disassembling: The player needs an empty hand!"));
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
				return;
			}
			break;
		}
	}

	FGameplayEffectSpecHandle EffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(SpeedReductionEffect, 1, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();
	EffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("GameplayEffectCaller.Speed"), ItemForDisassemblyData->MovementSpeedMultiplier);
	SpeedReductionEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec);

	Player->PlayAnimMontage(ItemForDisassemblyData->Animation);
	GetWorld()->GetTimerManager().SetTimer(DisassemblyTimer, this, &USGGA_AttemptDisassembleItem::OnDisassemblyFinished, ItemForDisassemblyData->ProcessLength, false);
}

void USGGA_AttemptDisassembleItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(SpeedReductionEffectSpecHandle);
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: EndAbility - Speed has been reset"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_AttemptDisassembleItem::OnDisassemblyFinished()
{
	EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->DestroyHittable(true);
	
	Player->StopAnimMontage(ItemForDisassemblyData->Animation);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
