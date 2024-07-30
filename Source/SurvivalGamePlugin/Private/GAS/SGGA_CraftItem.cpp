// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_CraftItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"


void USGGA_CraftItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AHumanCharacter* Player = CastChecked<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(World) == false"));
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
	FCraftingProcessDataStruct* ProductItemData = CraftingInfoTable->FindRow<FCraftingProcessDataStruct>(ProductItemName, "");
	
	FName EqupmentSocketName;
	AItemBase** ItemPointer = nullptr;
	if (ProductItemData->ItemsToDestroy.Contains(Player->SecondaryHandItem->GetItemDataRowName()))
	{
		Player->SecondaryHandItem->Destroy();
		ItemPointer = &Player->SecondaryHandItem;
		EqupmentSocketName = Player->GetSecondaryHandSocket();
	}
	if (ProductItemData->ItemsToDestroy.Contains(Player->MainHandItem->GetItemDataRowName()))
	{
		Player->MainHandItem->Destroy();
		ItemPointer = &Player->MainHandItem;
		EqupmentSocketName = Player->GetMainHandSocket();
	}

	AActor* NewItemActor = nullptr;
	if (HasAuthority(&ActivationInfo))
	{
		NewItemActor = World->SpawnActor<AActor>(ProductItemData->ItemActorClass, FVector(0, 0, 0), FRotator(0, 0, 0));
	}
	
	if (Cast<AItemBase>(NewItemActor))
	{
		if (ItemPointer == nullptr)
		{
			Cast<AItemBase>(NewItemActor)->OnItemUnequiped(Player);
		}
		else
		{
			Cast<AItemBase>(NewItemActor)->OnItemEquiped(Player, EqupmentSocketName);
			*ItemPointer = Cast<AItemBase>(NewItemActor);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
