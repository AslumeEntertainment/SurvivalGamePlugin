// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_AttemptDisassembleItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"


bool USGGA_AttemptDisassembleItem::bCanDisassembleItem(AItemBase* ItemForDisassembly, AItemBase* OtherEquipedItem)
{
	if (!IsValid(ItemForDisassembly))
	{
		UE_LOG(LogTemp, Warning, TEXT("Disassembling: Current item is not valid!"));
		return false;
	}

	FItemDataStruct* ItemData = ItemForDisassembly->ItemData.GetRow<FItemDataStruct>(TEXT("USGGA_AttemptDisassembleItem::AttemptDisassembleItem: Retrieving item data"));
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

void USGGA_AttemptDisassembleItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	AItemBase** ItemForDisassembly = nullptr;
	AItemBase** OtherItem = nullptr;
	FName MainItemSocket;
	FName IngredientitemSocket;

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

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(World) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	FItemDataStruct* IngredientParentItemData = (*ItemForDisassembly)->ItemData.DataTable->FindRow<FItemDataStruct>(ItemForDisassemblyData->IngredientParentItem, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility: Retrieving ingredient item data"));
	if (IngredientParentItemData && HasAuthority(&ActivationInfo))
	{
		AActor* NewItemActor = World->SpawnActor<AActor>(IngredientParentItemData->ItemActorClass, FVector(0, 0, 0), FRotator(0, 0, 0));
		if (Cast<AItemBase>(NewItemActor))
		{
			if (!IsValid(*OtherItem))
			{
				Cast<AItemBase>(NewItemActor)->OnItemEquiped(Player, IngredientitemSocket);
				*OtherItem = Cast<AItemBase>(NewItemActor);
			}
			else
			{
				Cast<AItemBase>(NewItemActor)->OnItemUnequiped(Player);
			}
		}
	}

	FItemDataStruct* MainParentItemData = (*ItemForDisassembly)->ItemData.DataTable->FindRow<FItemDataStruct>(ItemForDisassemblyData->MainParentItem, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility: Retrieving main parent item data"));
	if (MainParentItemData && HasAuthority(&ActivationInfo))
	{
		AActor* NewItemActor = World->SpawnActor<AActor>(MainParentItemData->ItemActorClass, FVector(0, 0, 0), FRotator(0, 0, 0));
		if (Cast<AItemBase>(NewItemActor))
		{
			(*ItemForDisassembly)->Destroy();
			Cast<AItemBase>(NewItemActor)->OnItemEquiped(Player, MainItemSocket);
			(*ItemForDisassembly) = Cast<AItemBase>(NewItemActor);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
