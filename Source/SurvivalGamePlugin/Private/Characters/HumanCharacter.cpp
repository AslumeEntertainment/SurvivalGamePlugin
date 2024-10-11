// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/HumanCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "InputActionValue.h"

#include "Interfaces/InteractableInterface.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/ItemBase.h"
#include "UI/Widgets/CraftingWindowWidget.h"
#include "UI/Widgets/ItemIconWidget.h"
#include "UI/HUD/InGameHUD.h"


void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (bIsRightHanded == false && EquipmentSlots.Contains(GetMainEquipmentSlotTag()) && EquipmentSlots.Contains(GetSecondaryEquipmentSlotTag()))
	{
		FName Temp = EquipmentSockets[GetMainEquipmentSlotTag()];
		EquipmentSockets[GetMainEquipmentSlotTag()] == EquipmentSockets[GetSecondaryEquipmentSlotTag()];
		EquipmentSockets[GetSecondaryEquipmentSlotTag()] = Temp;
	}
}

void AHumanCharacter::MainHandAction()
{
	FGameplayTag EventTag;
	FGameplayEventData EventData;

	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
	{
		EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Main");
	}
	else
	{
		FString ActionSuffix = IsValid(GetEquipedItemAtSlot(GetMainEquipmentSlotTag())) ? ".Item" : ".Attack";
		FString MirroringSuffix = bIsRightHanded ? ".Right" : ".Left";
		EventTag = FGameplayTag::RequestGameplayTag(FName("PlayerAction.Main" + ActionSuffix + MirroringSuffix));
	}

	SendGameplayEventToSelf(EventTag, EventData);
}
 
void AHumanCharacter::SecondaryHandAction()
{
	FGameplayTag EventTag;
	FGameplayEventData EventData;

	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
	{
		EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Secondary");
	}
	else
	{
		FString ActionSuffix = IsValid(GetEquipedItemAtSlot(GetSecondaryEquipmentSlotTag())) ? ".Item" : ".Attack";
		FString MirroringSuffix = bIsRightHanded ? ".Left" : ".Right";
		EventTag = FGameplayTag::RequestGameplayTag(FName("PlayerAction.Secondary" + ActionSuffix + MirroringSuffix));
	}

	SendGameplayEventToSelf(EventTag, EventData);
}