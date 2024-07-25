// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_CraftItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"

#include "Net/UnrealNetwork.h"


void USGGA_CraftItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	DOREPLIFETIME(USGGA_CraftItem, NewItemActor);
}

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

	FName ProductItemName = FName(*TriggerEventData->EventTag.ToString().RightChop(6));
	FDataTableRowHandle ProductItemRowData = Player->GetMainHandItem()->ItemData;
	ProductItemRowData.RowName = ProductItemName;
	
	FItemDataStruct* MainItemData = Player->GetMainHandItem()->ItemData.GetRow<FItemDataStruct>(TEXT("USGGA_CraftItem::ActivateAbility: Retrieving main hand item data"));
	if (!MainItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility MainItemData == nullptr"));
		return;
	}
	FItemDataStruct* ProductItemData = ProductItemRowData.GetRow<FItemDataStruct>(TEXT("USGGA_CraftItem::ActivateAbility: Retrieving product item data"));
	if (!ProductItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_CraftItem::ActivateAbility ProductItemData == nullptr"));
		return;
	}
	
	if (MainItemData->GetCraftingRecipeByProductName(ProductItemName).bIsIngredientDestroyed)
	{
		Player->GetSecondaryHandItem()->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Destroying the ingredient item"));
	}

	if (HasAuthority(&ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("Server: Spawning a new item and replacing the old one"));

		NewItemActor = World->SpawnActor<AActor>(ProductItemData->ItemActorClass, FVector(0, 0, 0), FRotator(0, 0, 0));
	}
	
	if (Cast<AItemBase>(NewItemActor))
	{
		Player->GetMainHandItem()->OnItemUnequiped(Player);
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Unequiping the old main item"));

		Player->GetMainHandItem()->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Destroying the old main item"));

		Cast<AItemBase>(NewItemActor)->OnItemEquiped(Player, Player->GetMainHandSocket());
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Equiping the new main item"));

		Player->SetMainHandItem(Cast<AItemBase>(NewItemActor));
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Setting MainHandItem to a valid AItemBase"));
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
