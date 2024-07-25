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
		if (IsValid(Player->GetMainHandItem()))
		{
			Player->GetMainHandItem()->OnItemUnequiped(Player);
			UE_LOG(LogTemp, Warning, TEXT("MainHandItem unequiped succesfully"));

			Player->SetMainHandItem(nullptr);
			UE_LOG(LogTemp, Warning, TEXT("MainHandItem set to nullptr"));

			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		else if (IsValid(Player->GetFocusedInteractableObject()) && Cast<AItemBase>(Player->GetFocusedInteractableObject()))
		{
			Player->SetMainHandItem(Cast<AItemBase>(Player->GetFocusedInteractableObject()));
			UE_LOG(LogTemp, Warning, TEXT("MainHandItem set to a valid AItemBase"));

			Player->GetMainHandItem()->OnItemEquiped(Player, Player->GetMainHandSocket());
			UE_LOG(LogTemp, Warning, TEXT("MainHandItem equiped succesfully"));

			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
	else
	{
		if (IsValid(Player->GetSecondaryHandItem()))
		{
			Player->GetSecondaryHandItem()->OnItemUnequiped(Player);
			UE_LOG(LogTemp, Warning, TEXT("SecondaryHandItem unequiped succesfully"));

			Player->SetSecondaryHandItem(nullptr);
			UE_LOG(LogTemp, Warning, TEXT("SecondaryHandItem set to nullptr"));

			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		else if (IsValid(Player->GetFocusedInteractableObject()) && Cast<AItemBase>(Player->GetFocusedInteractableObject()))
		{
			Player->SetSecondaryHandItem(Cast<AItemBase>(Player->GetFocusedInteractableObject()));
			UE_LOG(LogTemp, Warning, TEXT("SecondaryHandItem set to a valid AItemBase"));

			Player->GetSecondaryHandItem()->OnItemEquiped(Player, Player->GetSecondaryHandSocket());
			UE_LOG(LogTemp, Warning, TEXT("SecondaryHandItem equiped succesfully"));

			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
