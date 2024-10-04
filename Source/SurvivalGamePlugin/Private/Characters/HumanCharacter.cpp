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

	if (IsLocallyControlled())
	{
		UWorld* World = GetWorld();
		if (!IsValid(World))
		{
			UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::BeginPlay IsValid(World) == false"));
			return;
		}

		CraftingWindowWidget = CreateWidget<UCraftingWindowWidget>(UGameplayStatics::GetGameInstance(World), CraftingWindowWidgetClass);
	}
}

void AHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//EnhancedInputComponent->BindAction(CraftingAction, ETriggerEvent::Started, this, &AHumanCharacter::OpenCraftingWindow);
		//EnhancedInputComponent->BindAction(CraftingAction, ETriggerEvent::Completed, this, &AHumanCharacter::CloseCraftingWindow);
	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AHumanCharacter::MainHandAction()
{
	FGameplayEventData EventData;

	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
	{
		AbilitySystemComponent->Server_SendGameplayEventToSelf(FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Main"), EventData);//HandleGameplayEvent(FGameplayTag::RequestGameplayTag("PlayerAction.UnequipMain"), &EventData);
		return;
	}

	FString ActionSuffix = IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Main)) ? ".Item" : ".Attack";
	FString MirroringSuffix = bIsRightHanded ? ".Right" : ".Left";
	FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName("PlayerAction.Main" + ActionSuffix + MirroringSuffix));

	//rework after player state changes
	AbilitySystemComponent->HandleGameplayEvent(AbilityTag, &EventData);
}
 
void AHumanCharacter::SecondaryHandAction()
{
	FGameplayEventData EventData;

	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
	{
		AbilitySystemComponent->Server_SendGameplayEventToSelf(FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Secondary"), EventData);//HandleGameplayEvent(FGameplayTag::RequestGameplayTag("PlayerAction.UnequipSecondary"), &EventData);
		return;
	}

	FString ActionSuffix = IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)) ? ".Item" : ".Attack";
	FString MirroringSuffix = bIsRightHanded ? ".Left" : ".Right";
	FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName("PlayerAction.Secondary" + ActionSuffix + MirroringSuffix));

	//rework after player state changes
	AbilitySystemComponent->HandleGameplayEvent(AbilityTag, &EventData);
}

void AHumanCharacter::OpenCraftingWindow()
{
	/*if (!IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Main)) || !IsValid(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Player needs to have an item in both hands!"));
		return;
	}

	if (GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.bCanCraftWithItem(GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName()) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Player cannot craft anything with their current items!"));
		return;
	}

	if (IsValid(CraftingWindowWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::OpenCraftingWindow IsValid(CraftingWindowWidget) == false"));
		return;
	}

	APlayerController* PlayerCont = Cast<APlayerController>(GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::OpenCraftingWindow IsValid(PlayerCont) == false"));
		return;
	}

	for (FItemCraftingDataStruct CraftingRecipe : GetEquipedItemAtSlot(EEquipmentSlot::Main)->ItemData.CraftableItems)
	{
		if (CraftingRecipe.IngredientItemID == GetEquipedItemAtSlot(EEquipmentSlot::Secondary)->GetItemDataRowName())
		{
			UItemIconWidget* ItemIcon = CreateWidget<UItemIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), ItemIconWidgetClass);
			ItemIcon->SetItemImage(CraftingRecipe.ProductItemImage);
			ItemIcon->SetItemName(CraftingRecipe.ProductItemName);
			ItemIcon->IconClicked.AddDynamic(this, &AHumanCharacter::CraftItem);
			CraftingWindowWidget->AddCraftingRecipe(ItemIcon);
		}
	}

	SwitchInputMappingContext(EInputMappingContextState::Crafting);
	CraftingWindowWidget->AddToViewport();
	PlayerCont->SetInputMode(FInputModeGameAndUI());
	PlayerCont->bShowMouseCursor = true;
	*/
}

void AHumanCharacter::CloseCraftingWindow()
{
	if (IsValid(CraftingWindowWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::CloseCraftingWindow IsValid(CraftingWindowWidget) == false"));
		return;
	}
	APlayerController* PlayerCont = Cast<APlayerController>(GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::CloseCraftingWindow IsValid(PlayerCont) == false"));
		return;
	}

	CraftingWindowWidget->RemoveFromParent();
	CraftingWindowWidget->ClearRecipes();

	SwitchInputMappingContext(EInputMappingContextState::Default);

	PlayerCont->SetInputMode(FInputModeGameOnly());
	PlayerCont->bShowMouseCursor = false;
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


