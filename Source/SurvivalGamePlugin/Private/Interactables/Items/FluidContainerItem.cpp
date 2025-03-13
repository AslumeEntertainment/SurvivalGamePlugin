// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/FluidContainerItem.h"

#include "Components/EquipmentComponent.h"
#include "Components/FluidDataComponent.h"
#include "Objects/SG_TraceEmitterBase.h"
#include "Characters/InGameCharacter.h"


void AFluidContainerItem::Scoop()
{
	if (IsValid(ScoopTrace) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AFluidContainerItem::Scoop IsValid(ScoopTrace) == false"));
		return;
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetAttachParentActor());

	TArray<FHitResult> HitResults = ScoopTrace->CastTrace(GetActorTransform(), ActorsToIgnore, GetActorUpVector());

	for (FHitResult HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) == false)
		{
			continue;
		}
		UFluidDataComponent* FluidDataComp = HitActor->GetComponentByClass<UFluidDataComponent>();
		if (IsValid(FluidDataComp) == false)
		{
			continue;
		}
		TSubclassOf<AItemBase> NewItemClass = FluidDataComp->GetFluidData(this);
		if (IsValid(NewItemClass) == false)
		{
			continue;
		}

		UWorld* World = GetWorld();
		if (IsValid(World) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AFluidContainerItem::Scoop IsValid(World) == false"));
			return;
		}
		AItemBase* NewItem = World->SpawnActor<AItemBase>(NewItemClass, GetActorTransform());
		if (IsValid(NewItem) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AFluidContainerItem::Scoop IsValid(NewItem) == false"));
			return;
		}

		AInGameCharacter* AttachParent = Cast<AInGameCharacter>(GetAttachParentActor());
		if (IsValid(AttachParent))
		{
			UEquipmentComponent* EquipmentComp = AttachParent->GetEquipmentComponent();
			if (IsValid(EquipmentComp) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("AFluidContainerItem::Scoop IsValid(EquipmentComp) == false"));
				return;
			}
			EquipmentComp->EquipItem(NewItem);
		}

		Destroy();
		return;
	}
}
