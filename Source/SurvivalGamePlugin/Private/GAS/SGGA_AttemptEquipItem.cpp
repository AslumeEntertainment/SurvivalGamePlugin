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

	AItemBase** CurrentEquipedItem;
	FName SocketName;

	if (TriggerEventData->EventTag.GetTagName() == FName(TEXT("Equip.Main")))
	{
		CurrentEquipedItem = &Player->MainHandItem;
		SocketName = Player->GetMainHandSocket();
	}
	else 
	{
		CurrentEquipedItem = &Player->SecondaryHandItem;
		SocketName = Player->GetSecondaryHandSocket();
	}

	if (IsValid(*CurrentEquipedItem))
	{
		(*CurrentEquipedItem)->OnItemUnequiped(Player);
		*CurrentEquipedItem = nullptr;
	}
	else if (IsValid(Player->GetFocusedInteractableObject()) && Cast<AItemBase>(Player->GetFocusedInteractableObject()))
	{
		*CurrentEquipedItem = Cast<AItemBase>(Player->GetFocusedInteractableObject());
		(*CurrentEquipedItem)->OnItemEquiped(Player, SocketName);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
