// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_AttemptDisassembleItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"

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
	FItemDataStruct* MainHandItemData = Player->GetMainHandItem()->ItemData.GetRow<FItemDataStruct>(TEXT("USGGA_AttemptDisassembleItem::ActivateAbility: Retrieving main hand item data"));
	if (!MainHandItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility MainHandItemData == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	switch (MainHandItemData->DisassembleRequirement)
	{
		case EItemDisassembleRequirement::Unable:
			UE_LOG(LogTemp, Warning, TEXT("Disassembling: Equiped item cannot be disassembled!"));
			break;

		case EItemDisassembleRequirement::EmptyHand:
			if (IsValid(Player->GetSecondaryHandItem()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Disassembling: The player needs an empty hand!"));
				break;
			}

			UWorld* World = GetWorld();
			if (!IsValid(World))
			{
				UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility IsValid(World) == false"));
				break;
			}

			FItemDataStruct* IngredientParentItemData = Player->GetMainHandItem()->ItemData.DataTable->FindRow<FItemDataStruct>(MainHandItemData->IngredientParentItem, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility: Retrieving ingredient parent item data"));
			if (IngredientParentItemData && HasAuthority(&ActivationInfo))
			{
				AActor* NewItemActor = World->SpawnActor<AActor>(IngredientParentItemData->ItemActorClass, FVector(0, 0, 0), FRotator(0, 0, 0));
				if (Cast<AItemBase>(NewItemActor))
				{
					Cast<AItemBase>(NewItemActor)->OnItemEquiped(Player, Player->GetSecondaryHandSocket());
					Player->SetSecondaryHandItem(Cast<AItemBase>(NewItemActor));
				}
			}

			FItemDataStruct* MainParentItemData = Player->GetMainHandItem()->ItemData.DataTable->FindRow<FItemDataStruct>(MainHandItemData->MainParentItem, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility: Retrieving main parent item data"));
			if (MainParentItemData && HasAuthority(&ActivationInfo))
			{
				AActor* NewItemActor = World->SpawnActor<AActor>(MainParentItemData->ItemActorClass, FVector(0, 0, 0), FRotator(0, 0, 0));
				if (Cast<AItemBase>(NewItemActor))
				{
					Player->GetMainHandItem()->Destroy();
					Cast<AItemBase>(NewItemActor)->OnItemEquiped(Player, Player->GetMainHandSocket());
					Player->SetMainHandItem(Cast<AItemBase>(NewItemActor));
				}
			}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
