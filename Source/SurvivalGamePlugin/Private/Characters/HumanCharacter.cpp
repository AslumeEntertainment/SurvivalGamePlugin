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

	if (bIsRightHanded == false && EquipmentSlots.Contains(EEquipmentSlot::Main) && EquipmentSlots.Contains(EEquipmentSlot::Secondary))
	{
		FName Temp = EquipmentSockets[EEquipmentSlot::Main];
		EquipmentSockets[EEquipmentSlot::Main] == EquipmentSockets[EEquipmentSlot::Secondary];
		EquipmentSockets[EEquipmentSlot::Secondary] = Temp;
	}
}

void AHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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
		FString ActionSuffix = IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Main)) ? ".Item" : ".Attack";
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
		FString ActionSuffix = IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)) ? ".Item" : ".Attack";
		FString MirroringSuffix = bIsRightHanded ? ".Left" : ".Right";
		EventTag = FGameplayTag::RequestGameplayTag(FName("PlayerAction.Secondary" + ActionSuffix + MirroringSuffix));
	}

	SendGameplayEventToSelf(EventTag, EventData);
}

void AHumanCharacter::ShowPlayerInfo()
{
	FString Prefix;
	if (HasAuthority())
	{
		Prefix = "Server:";
	}
	else if (IsLocallyControlled())
	{
		Prefix = "Owning Client:";
	}
	else
	{
		Prefix = "Client:";
	}

	UE_LOG(LogTemp, Warning, TEXT("%s ===== Character stats ====="), *Prefix);

	if (GetFocusedInteractableObject() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject is a valid pointer"), *Prefix);

		IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(GetFocusedInteractableObject());
		if (InteractableActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject implements IInteractableInterface"), *Prefix);
			if (InteractableActor->bCanInteract(GetController()))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject can be interacted with"), *Prefix);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject cannot be interacted with, it is most likely equiped by a player"), *Prefix);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject == nullptr"), *Prefix);
	}

	if (IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Main)))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem is a valid pointer"), *Prefix);

		if (GetEquipedItemAtSlot(EEquipmentSlot::Main)->bCanInteract(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem can be interacted with, THIS SHOULD NOT BE POSSIBLE"), *Prefix);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem cannot be interacted with as it is currently equiped by the player"), *Prefix);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem is a %s"), *Prefix, *GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.ItemName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("%s Durability is %f out of %f - %f percent"), *Prefix, GetEquipedItemAtSlot(EEquipmentSlot::Main)->GetDurability(),
			GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.ItemMaxDurability , GetEquipedItemAtSlot(EEquipmentSlot::Main)->GetDurability() / GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.ItemMaxDurability);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem == nullptr"), *Prefix);
	}

	if (IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem is a valid pointer"), *Prefix);

		if (GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->bCanInteract(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem can be interacted with, THIS SHOULD NOT BE POSSIBLE"), *Prefix);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem cannot be interacted with as it is currently equiped by the player"), *Prefix);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem is a %s"), *Prefix, *GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->ItemData.ItemName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("%s Durability is %f out of %f - %f percent"), *Prefix, GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetDurability(),
			GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->ItemData.ItemMaxDurability, GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetDurability() / GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->ItemData.ItemMaxDurability);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem == nullptr"), *Prefix);
	}
}