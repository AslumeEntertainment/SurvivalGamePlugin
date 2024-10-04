// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_AttemptDisassembleItem.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Net/UnrealNetwork.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/HumanCharacter.h"
#include "Animation/SG_AnimInstance.h"


void USGGA_AttemptDisassembleItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USGGA_AttemptDisassembleItem, MainItemActor);
	DOREPLIFETIME(USGGA_AttemptDisassembleItem, IngredientItemActor);
}

void USGGA_AttemptDisassembleItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	OldSpeed = ActorInfo->AbilitySystemComponent->GetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute());

	AItemBase* ItemForDisassembly = Player->GetEquipedItemAtSlot(EEquipmentSlot::Main);
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
			if (IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
			{
				UE_LOG(LogTemp, Warning, TEXT("Disassembling: The player needs an empty hand!"));
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			}
			break;
		}
	}

	AnimInst = Cast<USG_AnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility IsValid(AnimInst) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (Player->bIsRightHanded == false)
	{
		AnimInst->bIsMirrored = true;
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		SpawnNewItems();
	}

	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute(), OldSpeed * ItemForDisassemblyData->MovementSpeedMultiplier);

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
	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_CharacterAttributeSet::GetSpeedAttribute(), OldSpeed);
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: EndAbility - Speed has been reset"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_AttemptDisassembleItem::SpawnNewItems()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::SpawnNewItems IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	MainItemActor = World->SpawnActor<AItemBase>(ItemForDisassemblyData->MainParentItem, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(MainItemActor))
	{
		MainItemActor->SetActorScale3D(FVector(0.0001, 0.0001, 0.0001));
	}

	IngredientItemActor = World->SpawnActor<AItemBase>(ItemForDisassemblyData->IngredientParentItem, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(IngredientItemActor))
	{
		IngredientItemActor->SetActorScale3D(FVector(0.0001, 0.0001, 0.0001));
	}

	UE_LOG(LogTemp, Warning, TEXT("USGGA_AttemptDisassembleItem::SpawnNewItems - New items have been spawned and kept hidden until ability has finished"));
}

void USGGA_AttemptDisassembleItem::OnAnimMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: Ability has been completed, resetting speed"));

	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_AttemptDisassembleItem::OnAnimMontageCanceled()
{
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: Ability has been canceled, resetting speed"));

	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_AttemptDisassembleItem::OnDisassemblyFinished(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("USGGA_AttemptDisassembleItem::OnDisassemblyFinished - Gameplay Event recieved"));
	Player->GetEquipedItemAtSlot(EEquipmentSlot::Main)->DestroyItem();

	if (IsValid(MainItemActor))
	{
		MainItemActor->OnItemEquiped(Player, Player->GetEquipmentSocketOfType(EEquipmentSlot::Main));
		Player->SetEquipedItemAtSlot(EEquipmentSlot::Main, MainItemActor);
	}

	if (IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)) == false && IsValid(IngredientItemActor))
	{
		IngredientItemActor->OnItemEquiped(Player, Player->GetEquipmentSocketOfType(EEquipmentSlot::Secondary));
		Player->SetEquipedItemAtSlot(EEquipmentSlot::Secondary, IngredientItemActor);
	}

	else if (IsValid(Player->GetEquipedItemAtSlot(EEquipmentSlot::Secondary)) && IsValid(IngredientItemActor))
	{
		IngredientItemActor->SetActorLocation(Player->GetPlayerLookAtLocation(100.f));
		IngredientItemActor->SetActorRotation(Player->GetActorRotation());
	}
	
	UE_LOG(LogTemp, Warning, TEXT("USGGA_AttemptDisassembleItem::OnDisassemblyFinished - Disassembly process finished!"));
}
