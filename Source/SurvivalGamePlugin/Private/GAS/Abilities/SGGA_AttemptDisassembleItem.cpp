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
	}
	Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	EquipmentComp = Player->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility IsValid(EquipmentComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AItemBase* ItemForDisassembly = EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag());
	if (IsValid(ItemForDisassembly) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility IsValid(ItemForDisassembly) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	ItemForDisassemblyData = DisassemlyInfoTable->FindRow<FDisassemblyProcessDataStruct>(ItemForDisassembly->GetItemDataRowName(), "");
	if (ItemForDisassemblyData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility ItemForDisassemblyData == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	switch (ItemForDisassemblyData->DisassembleRequirement)
	{
		case EItemDisassembleRequirement::Unable:
		{
			UE_LOG(LogTemp, Warning, TEXT("Disassembling: Current item cannot be disassembled!"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
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
			}
			break;
		}
	}

	if (Player->IsRightHanded() == false)
	{
		Player->SetIsMirrored(true);
	}

	FGameplayEffectSpecHandle EffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(SpeedReductionEffect, 1, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();
	EffectSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("GameplayEffectCaller.Speed"), ItemForDisassemblyData->MovementSpeedMultiplier);
	SpeedReductionEffectSpecHandle = CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ItemForDisassemblyData->Animation);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnAnimMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Notify.FinishDisassembly"), nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnDisassemblyFinished);
	WaitGameplayEventTask->ReadyForActivation();
}

void USGGA_AttemptDisassembleItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	/*if (bWasCancelled && IsValid(MainItemActor) && MainItemActor->GetActorScale() == FVector(0.0001, 0.0001, 0.0001))
	{
		MainItemActor->Destroy();
	}
	if (bWasCancelled && IsValid(IngredientItemActor) && IngredientItemActor->GetActorScale() == FVector(0.0001, 0.0001, 0.0001))
	{
		IngredientItemActor->Destroy();
	}*/
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(SpeedReductionEffectSpecHandle);
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: EndAbility - Speed has been reset"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_AttemptDisassembleItem::OnAnimMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: Ability has been completed, resetting speed"));

	Player->SetIsMirrored(false);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_AttemptDisassembleItem::OnAnimMontageCanceled()
{
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: Ability has been canceled, resetting speed"));

	Player->SetIsMirrored(false);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_AttemptDisassembleItem::OnDisassemblyFinished(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("USGGA_AttemptDisassembleItem::OnDisassemblyFinished - Gameplay Event recieved"));
	EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->DestroyItem();

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::SpawnNewItems IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	float ItemDurabilityPercent = EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->GetDurability() /
		EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag())->ItemData.ItemMaxDurability;

	MainItemActor = World->SpawnActor<AItemBase>(ItemForDisassemblyData->MainParentItem, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(MainItemActor))
	{
		MainItemActor->SetDurability(MainItemActor->GetDurability() * ItemDurabilityPercent);
		MainItemActor->OnItemEquiped(Player, EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetMainEquipmentSlotTag()));
		EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetMainEquipmentSlotTag(), MainItemActor);
	}

	IngredientItemActor = World->SpawnActor<AItemBase>(ItemForDisassemblyData->IngredientParentItem, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(IngredientItemActor) && IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())) == false)
	{
		IngredientItemActor->SetDurability(IngredientItemActor->GetDurability() * ItemDurabilityPercent);
		IngredientItemActor->OnItemEquiped(Player, EquipmentComp->GetEquipmentSocketWithTag(EquipmentComp->GetSecondaryEquipmentSlotTag()));
		EquipmentComp->SetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag(), IngredientItemActor);
	}
	else if (IsValid(IngredientItemActor) && IsValid(EquipmentComp->GetEquipedItemAtSlot(EquipmentComp->GetSecondaryEquipmentSlotTag())))
	{
		IngredientItemActor->SetDurability(IngredientItemActor->GetDurability() * ItemDurabilityPercent);
		IngredientItemActor->SetActorLocation(Player->GetPlayerLookAtLocation(100.f));
		IngredientItemActor->SetActorRotation(Player->GetActorRotation());
	}
	
	UE_LOG(LogTemp, Warning, TEXT("USGGA_AttemptDisassembleItem::OnDisassemblyFinished - Disassembly process finished!"));
}
