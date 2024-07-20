// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SGGA_AttemptEquipItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"

void USGGA_AttemptEquipItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptEquipItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	AHumanCharacter* Player = CastChecked<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptEquipItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	/*
	AItemBase** EquipedItem;
	FName SocketName;

	if (bIsUsingMainHand)
	{
		EquipedItem = &(Player->MainHandItem);
		SocketName = Player->GetMainHandSocket();
	}
	else
	{
		EquipedItem = &(Player->SecondaryHandItem);
		SocketName = Player->GetSecondaryHandSocket();
	}

	if (IsValid(*EquipedItem))
	{
		(*EquipedItem)->OnItemUnequiped(Player);
		*EquipedItem = nullptr;
	}
	else if (IsValid(Player->GetFocusedInteractableObject()) && Cast<AItemBase>(Player->GetFocusedInteractableObject()))
	{
		(*EquipedItem)->OnItemEquiped(Player, SocketName);
		*EquipedItem = Cast<AItemBase>(Player->GetFocusedInteractableObject());
	}
	*/
	if (bIsUsingMainHand) 
	{
		if (IsValid(Player->MainHandItem))
		{
			Player->MainHandItem->OnItemUnequiped(Player);
			Player->MainHandItem = nullptr;
		}
		else if (IsValid(Player->GetFocusedInteractableObject()) && Cast<AItemBase>(Player->GetFocusedInteractableObject()))
		{
			Player->MainHandItem = Cast<AItemBase>(Player->GetFocusedInteractableObject());
			Player->MainHandItem->OnItemEquiped(Player, Player->GetMainHandSocket());
		}
	}
	else
	{
		if (IsValid(Player->SecondaryHandItem))
		{
			Player->SecondaryHandItem->OnItemUnequiped(Player);
			Player->SecondaryHandItem = nullptr;
		}
		else if (IsValid(Player->GetFocusedInteractableObject()) && Cast<AItemBase>(Player->GetFocusedInteractableObject()))
		{
			Player->SecondaryHandItem = Cast<AItemBase>(Player->GetFocusedInteractableObject());
			Player->SecondaryHandItem->OnItemEquiped(Player, Player->GetSecondaryHandSocket());
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
