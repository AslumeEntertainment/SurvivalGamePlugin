// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/HumanCharacter.h"
#include "Interfaces/InteractableInterface.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Interactables/Items/ItemBase.h"
#include "UI/HUD/InGameHUD.h"
#include "Net/UnrealNetwork.h"

#include "GAS/SG_GameplayAbilityTargetData.h"
#include "GAS/SG_CharacterAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"


//void AHumanCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	//DOREPLIFETIME_CONDITION_NOTIFY(AHumanCharacter, MainHandItem, COND_None, REPNOTIFY_Always);
//	//DOREPLIFETIME_CONDITION_NOTIFY(AHumanCharacter, SecondaryHandItem, COND_None, REPNOTIFY_Always);
//}

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
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Triggered, this, &AHumanCharacter::MainHandAction);
		EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Triggered, this, &AHumanCharacter::SecondaryHandAction);

		EnhancedInputComponent->BindAction(UnequipAction, ETriggerEvent::Started, this, &AHumanCharacter::StartUnequipItem);
		EnhancedInputComponent->BindAction(UnequipAction, ETriggerEvent::Completed, this, &AHumanCharacter::CancelUnequipItem);
		
		EnhancedInputComponent->BindAction(AbilityAction, ETriggerEvent::Triggered, this, &AHumanCharacter::UseDebugAbility);

		EnhancedInputComponent->BindAction(CraftingAction, ETriggerEvent::Started, this, &AHumanCharacter::OpenCraftingWindow);
		EnhancedInputComponent->BindAction(CraftingAction, ETriggerEvent::Completed, this, &AHumanCharacter::CloseCraftingWindow);

		EnhancedInputComponent->BindAction(DisassembleAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptDisassembleItem);
	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AHumanCharacter::MainHandAction()
{
	if (IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Main)))
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
		{
			AbilitySystemComponent->Server_SendGameplayEventToSelf(FGameplayTag::RequestGameplayTag("PlayerAction.UnequipMain"), FGameplayEventData());
		}
		else
		{
			FString ItemAbilityTagSuffix = bIsRightHanded ? ".Right" : ".Left";

			FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName(GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.ItemAbilityTag.ToString() + ItemAbilityTagSuffix));
			FGameplayEventData EventData;

			FSG_GameplayAbilityTargetData* CustomTargetData = new FSG_GameplayAbilityTargetData();
			CustomTargetData->ItemActor = GetEquipedItemAtSlot(EEquipmentSlot::Main);
			EventData.TargetData.Add(CustomTargetData);

			AbilitySystemComponent->Server_SendGameplayEventToSelf(AbilityTag, EventData);
		}
	}
	else
	{
		FGameplayTag HandAbilityTag = bIsRightHanded ? FGameplayTag::RequestGameplayTag(FName("Combat.Punch.Right")) : FGameplayTag::RequestGameplayTag(FName("Combat.Punch.Left"));

		//HasAuthority() ? AbilitySystemComponent->Server_SendGameplayEventToSelf(HandAbilityTag, FGameplayEventData()) : AbilitySystemComponent->Client_SendGameplayEventToSelf(HandAbilityTag, FGameplayEventData());
		AbilitySystemComponent->Server_SendGameplayEventToSelf(HandAbilityTag, FGameplayEventData());
	}
}
 
void AHumanCharacter::SecondaryHandAction()
{
	if (IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
		{
			AbilitySystemComponent->Server_SendGameplayEventToSelf(FGameplayTag::RequestGameplayTag("PlayerAction.UnequipSecondary"), FGameplayEventData());
		}
		else
		{
			FString ItemAbilityTagSuffix = bIsRightHanded ? ".Left" : ".Right";

			FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->ItemData.ItemAbilityTag.ToString() + ItemAbilityTagSuffix));
			FGameplayEventData EventData;

			FSG_GameplayAbilityTargetData* CustomTargetData = new FSG_GameplayAbilityTargetData();
			CustomTargetData->ItemActor = GetEquipedItemAtSlot(EEquipmentSlot::Secondary);
			EventData.TargetData.Add(CustomTargetData);

			AbilitySystemComponent->Server_SendGameplayEventToSelf(AbilityTag, EventData);
		}
	}
	else
	{
		FGameplayTag HandAbilityTag = bIsRightHanded ? FGameplayTag::RequestGameplayTag(FName("Combat.Punch.Left")) : FGameplayTag::RequestGameplayTag(FName("Combat.Punch.Right"));

		//HasAuthority() ? AbilitySystemComponent->Server_SendGameplayEventToSelf(HandAbilityTag, FGameplayEventData()) : AbilitySystemComponent->Client_SendGameplayEventToSelf(HandAbilityTag, FGameplayEventData());
		AbilitySystemComponent->Server_SendGameplayEventToSelf(HandAbilityTag, FGameplayEventData());
	}
}

void AHumanCharacter::StartUnequipItem()
{
	if (!IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Main)) && !IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
	{
		UE_LOG(LogTemp, Warning, TEXT("No items to unequip"));
		return;
	}
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::Unequip);
}

void AHumanCharacter::CancelUnequipItem()
{
	SendLocalInputToAbilitySystemComponent(false, EGameplayAbilityInputID::Unequip);
}

void AHumanCharacter::UseDebugAbility()
{
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::Action);
}

void AHumanCharacter::OpenCraftingWindow()
{
	if (!IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Main)) || !IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Player needs to have an item in both hands!"));
		return;
	}

	if (GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.bCanCraftWithItem(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName()) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Player cannot craft anything with their current items!"));
		return;
	}
	
	CraftingWindowOpenDelegate.Broadcast(GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData, GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName());
}

void AHumanCharacter::CloseCraftingWindow()
{
	CraftingWindowCloseDelegate.Broadcast();
}

void AHumanCharacter::AttemptDisassembleItem()
{
	if (!IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Main)) && !IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Disassembing: Player is not holding any items!"));
		return;
	}

	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::Disassemble);
}

void AHumanCharacter::CraftItem(FText ProductItemName)
{
	CloseCraftingWindow();

	FString GameplayTagString = FString(TEXT("Craft.")) + ProductItemName.ToString();
	GameplayTagString.RemoveSpacesInline();
	FGameplayTag ProductNameTag = FGameplayTag::RequestGameplayTag(FName(GameplayTagString));

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ProductNameTag, FGameplayEventData());
}

void AHumanCharacter::SwitchInputMappingContext(EInputMappingContextState InputContextState)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::SwitchInputMappingContext IsValid(PlayerController) == false"));
		return;
	}
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!IsValid(Subsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::SwitchInputMappingContext IsValid(Subsystem) == false"));
		return;
	}

	switch (InputContextState)
	{
	case EInputMappingContextState::Default:
		Subsystem->RemoveMappingContext(CraftingWindowOnlyMappingContext);
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
		break;

	case EInputMappingContextState::Crafting:
		Subsystem->RemoveMappingContext(DefaultMappingContext);
		Subsystem->AddMappingContext(CraftingWindowOnlyMappingContext, 0);
		break;
	}
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
		if (GetEquipedItemAtSlot(EEquipmentSlot::Main)->GetActorEnableCollision())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem has collisions enabled"), *Prefix);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem does not have collisions enabled"), *Prefix);
		}


		UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem is a %s"), *Prefix, *GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.ItemName.ToString());
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
		if (GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetActorEnableCollision())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem has collisions enabled"), *Prefix);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem does not have collisions enabled"), *Prefix);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem is a %s"), *Prefix, *GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->ItemData.ItemName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem == nullptr"), *Prefix);
	}
}


