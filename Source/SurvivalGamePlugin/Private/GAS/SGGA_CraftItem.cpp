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

	FName ProductItemName = FName(*TriggerEventData->EventTag.ToString().RightChop(6)); //Removing the "Craft." prefix
	FDataTableRowHandle ProductItemRowData = Player->MainHandItem->ItemData;
	ProductItemRowData.RowName = ProductItemName;
	
	FItemDataStruct* MainItemData = Player->MainHandItem->ItemData.GetRow<FItemDataStruct>(TEXT("USGGA_CraftItem::ActivateAbility: Retrieving main hand item data"));
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
	
	FName EqupmentSocketName;
	AItemBase** ItemPointer = nullptr;
	if (MainItemData->GetCraftingRecipeByProductName(ProductItemName).bIsIngredientDestroyed)
	{
		Player->SecondaryHandItem->Destroy();
		ItemPointer = &Player->SecondaryHandItem;
		EqupmentSocketName = Player->GetSecondaryHandSocket();
	}
	if (MainItemData->GetCraftingRecipeByProductName(ProductItemName).bIsMainItemDestroyed)
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
		Cast<AItemBase>(NewItemActor)->OnItemEquiped(Player, EqupmentSocketName);
		*ItemPointer = Cast<AItemBase>(NewItemActor);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
