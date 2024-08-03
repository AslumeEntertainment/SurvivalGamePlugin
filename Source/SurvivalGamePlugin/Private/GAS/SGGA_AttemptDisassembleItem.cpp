// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_AttemptDisassembleItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


void USGGA_AttemptDisassembleItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = CastChecked<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (bCanDisassembleItem(Player->MainHandItem, Player->SecondaryHandItem))
	{
		ItemForDisassembly = &Player->MainHandItem;
		OtherItem = &Player->SecondaryHandItem;
		MainItemSocket = Player->GetMainHandSocket();
		IngredientitemSocket = Player->GetSecondaryHandSocket();
	}
	else if (bCanDisassembleItem(Player->SecondaryHandItem, Player->MainHandItem))
	{
		ItemForDisassembly = &Player->SecondaryHandItem;
		OtherItem = &Player->MainHandItem;
		MainItemSocket = Player->GetSecondaryHandSocket();
		IngredientitemSocket = Player->GetMainHandSocket();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Disassembling: No item can be disassembled!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Player->MultiplyMaxWalkingSpeed(ItemForDisassemblyData->MovementSpeedMultiplier);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ItemForDisassemblyData->Animation);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnAnimMontageBlendOut);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();
}

bool USGGA_AttemptDisassembleItem::bCanDisassembleItem(AItemBase* MainItemForDisassembly, AItemBase* OtherEquipedItem)
{
	if (!IsValid(MainItemForDisassembly))
	{
		UE_LOG(LogTemp, Warning, TEXT("Disassembling: Current item is not valid!"));
		return false;
	}

	FDisassemblyProcessDataStruct* ItemData = DisassemlyInfoTable->FindRow<FDisassemblyProcessDataStruct>(MainItemForDisassembly->GetItemDataRowName(), "");
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::AttemptDisassembleItem ItemData == nullptr"));
		return false;
	}

	switch (ItemData->DisassembleRequirement)
	{
		case EItemDisassembleRequirement::Unable:
			UE_LOG(LogTemp, Warning, TEXT("Disassembling: Current item cannot be disassembled!"));
			return false;

		case EItemDisassembleRequirement::Always:
			break;

		case EItemDisassembleRequirement::EmptyHand:
			if (IsValid(OtherEquipedItem))
			{
				UE_LOG(LogTemp, Warning, TEXT("Disassembling: The player needs an empty hand!"));
				return false;
			}
			break;
	}

	ItemForDisassemblyData = ItemData;
	return true;
}

void USGGA_AttemptDisassembleItem::OnAnimMontageBlendOut()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::OnAnimMontageBlendOut IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		AActor* IngredientItemActor = World->SpawnActor<AActor>(ItemForDisassemblyData->IngredientParentItem, FVector(0, 0, 0), FRotator(0, 0, 0));
		if (Cast<AItemBase>(IngredientItemActor))
		{
			if (!IsValid(*OtherItem))
			{
				Cast<AItemBase>(IngredientItemActor)->OnItemEquiped(Player, IngredientitemSocket);
				*OtherItem = Cast<AItemBase>(IngredientItemActor);
			}
			else
			{
				Cast<AItemBase>(IngredientItemActor)->OnItemUnequiped(Player);
			}
		}

		AActor* MainItemActor = World->SpawnActor<AActor>(ItemForDisassemblyData->MainParentItem, FVector(0, 0, 0), FRotator(0, 0, 0));
		if (Cast<AItemBase>(MainItemActor))
		{
			(*ItemForDisassembly)->Destroy();
			Cast<AItemBase>(MainItemActor)->OnItemEquiped(Player, MainItemSocket);
			(*ItemForDisassembly) = Cast<AItemBase>(MainItemActor);
		}
	}

	Player->ResetWalkingSpeed();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_AttemptDisassembleItem::OnAnimMontageCanceled()
{
	Player->ResetWalkingSpeed();
	UE_LOG(LogTemp, Warning, TEXT("Disassembly animation was unexpectedly canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
