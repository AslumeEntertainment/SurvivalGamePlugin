// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_CraftItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


void USGGA_CraftItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	if (!IsValid(Player->MainHandItem))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player->MainHandItem) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	if (!IsValid(Player->SecondaryHandItem))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player->SecondaryHandItem) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	FName ProductItemName = FName(*TriggerEventData->EventTag.ToString().RightChop(6)); //Removing the "Craft." prefix
	ProductItemData = CraftingInfoTable->FindRow<FCraftingProcessDataStruct>(ProductItemName, "");
	if (ProductItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility ProductItemData == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	Player->MultiplyMaxWalkingSpeed(ProductItemData->MovementSpeedMultiplier);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ProductItemData->Animation);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USGGA_CraftItem::OnAnimMontageBlendOut);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_CraftItem::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();
}

void USGGA_CraftItem::OnAnimMontageBlendOut()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::OnAnimMontageBlendOut IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	FName EqupmentSocketName;
	AItemBase** ItemPointer = nullptr;
	if (ProductItemData->ItemsToDestroy.Contains(Player->SecondaryHandItem->GetItemDataRowName()))
	{
		Player->SecondaryHandItem->DestroyItem();
		ItemPointer = &Player->SecondaryHandItem;
		EqupmentSocketName = Player->GetSecondaryHandSocket();
	}
	if (ProductItemData->ItemsToDestroy.Contains(Player->MainHandItem->GetItemDataRowName()))
	{
		Player->MainHandItem->DestroyItem();
		ItemPointer = &Player->MainHandItem;
		EqupmentSocketName = Player->GetMainHandSocket();
	}

	AActor* NewItemActor = nullptr;
	if (HasAuthority(&CurrentActivationInfo))
	{
		NewItemActor = World->SpawnActor<AActor>(ProductItemData->ItemActorClass, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	}

	if (Cast<AItemBase>(NewItemActor))
	{
		if (ItemPointer != nullptr)
		{
			Cast<AItemBase>(NewItemActor)->OnItemEquiped(Player, EqupmentSocketName);
			*ItemPointer = Cast<AItemBase>(NewItemActor);
		}
	}

	Player->ResetWalkingSpeed();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_CraftItem::OnAnimMontageCanceled()
{
	Player->ResetWalkingSpeed();
	UE_LOG(LogTemp, Warning, TEXT("Crafting animation was unexpectedly canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

