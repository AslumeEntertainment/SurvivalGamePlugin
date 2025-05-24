// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/CollectableActor.h"

#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"
#include "Components/EquipmentComponent.h"


ACollectableActor::ACollectableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

bool ACollectableActor::InteractRequest(AController* InteractingCharacter)
{
	if (HasAuthority() == false)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("CollectableActor::InteractRequest IsValid(World) == false"));
		return false;
	}
	AInGameCharacter* InteractingPawn = Cast<AInGameCharacter>(InteractingCharacter->GetPawn());
	if (IsValid(InteractingPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("CollectableActor::InteractRequest IsValid(InteractingPawn) == false"));
		return false;
	}
	UEquipmentComponent* EquipmentComp = InteractingPawn->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("CollectableActor::InteractRequest IsValid(EquipmentComp) == false"));
		return false;
	}

	AItemBase* Item = World->SpawnActor<AItemBase>(ItemToCollect, GetActorLocation(), GetActorRotation());
	if (IsValid(Item) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("CollectableActor::InteractRequest IsValid(Item) == false"));
		return false;
	}

	EquipmentComp->EquipItem(Item);
	DestroyHittable();

	return true;
}

bool ACollectableActor::bCanInteract(AController* InteractingCharacter) const
{
	if (IsValid(ItemToCollect) == false)
	{
		return false;
	}

	AItemBase* Item = ItemToCollect->GetDefaultObject<AItemBase>();
	if (IsValid(Item) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("CollectableActor::bCanInteract IsValid(Item) == false"));
		return false;
	}

	return Item->bCanInteract(InteractingCharacter);
}

FText ACollectableActor::GetInteractableName()
{
	return CollectableName;
}
