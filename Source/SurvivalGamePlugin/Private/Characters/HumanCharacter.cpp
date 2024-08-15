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

#include "AbilitySystemBlueprintLibrary.h"


void AHumanCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AHumanCharacter, FocusedInteractableObject, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHumanCharacter, MainHandItem, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHumanCharacter, SecondaryHandItem, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHumanCharacter, ActiveItem, COND_None, REPNOTIFY_Always);
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled()) 
	{
		return;
	}
	EmitInteractionChecker();
}

void AHumanCharacter::ServerSetFocusedInteractable_Implementation(UObject* InFocusedInteractable)
{
	FocusedInteractableObject = InFocusedInteractable;
}

void AHumanCharacter::EmitInteractionChecker()
{
	FVector StartTrace = GetFirstPersonCamera()->GetComponentLocation();
	FVector EndTrace = GetFirstPersonCamera()->GetForwardVector() * InteractionRange + StartTrace;

	const FName TraceTag("InteractableTraceTag");

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::EmitInteractionChecker IsValid(World) == false"));
		return;
	}

	FHitResult HitResult;
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);
	CQP.TraceTag = TraceTag;

	if (bShouldDrawDebug)
	{
		World->DebugDrawTraceTag = TraceTag;
	}
	World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_GameTraceChannel1, CQP);

	CheckForInteractables(HitResult);
}

void AHumanCharacter::CheckForInteractables(FHitResult HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	UPrimitiveComponent* HitComponent = HitResult.GetComponent();
	IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(HitActor);

	if (FocusedInteractableObject == HitActor)
	{
		return;
	}
	if (InteractableActor == nullptr) 
	{
		if (FocusedInteractableObject == nullptr)
		{
			return;
		}
		UActorComponent* InteractableMeshComponent = Cast<AActor>(FocusedInteractableObject)->GetComponentByClass(UMeshComponent::StaticClass());
		if (Cast<UMeshComponent>(InteractableMeshComponent))
		{
			Cast<UMeshComponent>(InteractableMeshComponent)->SetOverlayMaterial(nullptr);
		}

		ServerSetFocusedInteractable(nullptr);
		return;
	}
	if (!InteractableActor->bCanInteract(GetController()))
	{
		return;
	}
	if (FocusedInteractableObject != nullptr)
	{
		UActorComponent* InteractableMeshComponent = Cast<AActor>(FocusedInteractableObject)->GetComponentByClass(UMeshComponent::StaticClass());
		if (Cast<UMeshComponent>(InteractableMeshComponent))
		{
			Cast<UMeshComponent>(InteractableMeshComponent)->SetOverlayMaterial(nullptr);
		}
	}

	UMeshComponent* HitMeshComponent = Cast<UMeshComponent>(HitActor->GetComponentByClass(UMeshComponent::StaticClass()));
	if (HitMeshComponent != nullptr)
	{
		HitMeshComponent->SetOverlayMaterial(InteractableOverlayMaterial);
	}

	ServerSetFocusedInteractable(HitActor);
}

void AHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(EquipMainHandAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptInteraction);
		EnhancedInputComponent->BindAction(EquipSecondaryHandAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptEquipWithSecondaryHand);
		
		EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptUseMainHandItem);
		EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptUseSecondaryHandItem);
		
		EnhancedInputComponent->BindAction(AbilityAction, ETriggerEvent::Triggered, this, &AHumanCharacter::UseAbility);

		EnhancedInputComponent->BindAction(CraftingAction, ETriggerEvent::Started, this, &AHumanCharacter::OpenCraftingWindow);
		EnhancedInputComponent->BindAction(CraftingAction, ETriggerEvent::Completed, this, &AHumanCharacter::CloseCraftingWindow);

		EnhancedInputComponent->BindAction(DisassembleAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptDisassembleItem);
	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AHumanCharacter::AttemptInteraction()
{
	if (FocusedInteractableObject == nullptr)
	{
		if (bShouldDrawDebug == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("COULD NOT INTERACT WITH EMPTY FOCUS"));
		}
		return;
	}

	AItemBase* Item = Cast<AItemBase>(FocusedInteractableObject);
	if (IsValid(Item) == true)
	{
		AttemptEquipWithMainHand();
		return;
	}

	IInteractableInterface* Interactable = Cast<IInteractableInterface>(FocusedInteractableObject);
	if (Interactable != nullptr)
	{
		Interactable->InteractRequest();
		return;
	};

}

void AHumanCharacter::AttemptEquipWithMainHand()
{
	FGameplayTag EquipAbilityTag;
	EquipAbilityTag.FromExportString("Equip.Main");
	if (!EquipAbilityTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT(" AHumanCharacter::AttemptEquipWithMainHand EquipAbilityTag.IsValid() == false"));
		return;
	}
	FGameplayEventData EventData;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EquipAbilityTag, EventData);
}

void AHumanCharacter::AttemptEquipWithSecondaryHand()
{
	FGameplayTag EquipAbilityTag;
	EquipAbilityTag.FromExportString("Equip.Secondary");
	if (!EquipAbilityTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT(" AHumanCharacter::AttemptEquipWithSecondaryHand EquipAbilityTag.IsValid() == false"));
		return;
	}
	FGameplayEventData EventData;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EquipAbilityTag, EventData);
}

void AHumanCharacter::AttemptUseMainHandItem()
{
	if (IsValid(MainHandItem))
	{
		MainHandItem->Use(this);
	}
	else
	{
		FGameplayTag HandAbilityTag;
		if (bIsRightHanded)
		{
			HandAbilityTag.FromExportString("Punch.Right");
		}
		else
		{
			HandAbilityTag.FromExportString("Punch.Left");
		}
		
		if (!HandAbilityTag.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT(" AHumanCharacter::AttemptUseMainHandItem HandAbilityTag.IsValid() == false"));
			return;
		}

		FGameplayEventData EventData;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, HandAbilityTag, EventData);
	}
}

void AHumanCharacter::AttemptUseSecondaryHandItem()
{
	if (IsValid(SecondaryHandItem))
	{
		SecondaryHandItem->Use(this);
	}
	else
	{
		FGameplayTag HandAbilityTag;
		if (bIsRightHanded)
		{
			HandAbilityTag.FromExportString("Punch.Left");
		}
		else
		{
			HandAbilityTag.FromExportString("Punch.Right");
		}

		if (!HandAbilityTag.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT(" AHumanCharacter::AttemptUseSecondaryHandItem HandAbilityTag.IsValid() == false"));
			return;
		}

		FGameplayEventData EventData;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, HandAbilityTag, EventData);
		//SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::Punch);
	}
}

void AHumanCharacter::UseAbility()
{
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::Action);
}

void AHumanCharacter::OpenCraftingWindow()
{
	if (!IsValid(MainHandItem) || !IsValid(SecondaryHandItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Player needs to have an item in both hands!"));
		return;
	}

	if (!MainHandItem->ItemData.bCanCraftWithItem(SecondaryHandItem->GetItemDataRowName()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Player cannot craft anything with their current items!"));
		return;
	}
	
	CraftingWindowOpenDelegate.Broadcast(MainHandItem->ItemData, SecondaryHandItem->GetItemDataRowName());
}

void AHumanCharacter::CloseCraftingWindow()
{
	CraftingWindowCloseDelegate.Broadcast();
}

void AHumanCharacter::AttemptDisassembleItem()
{
	if (!IsValid(MainHandItem) && !IsValid(SecondaryHandItem))
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
	FGameplayTag ProductNameTag;
	ProductNameTag.FromExportString(GameplayTagString);

	if (!ProductNameTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT(" AHumanCharacter::CraftItem ProductNameTag.IsValid() == false"));
		return;
	}

	FGameplayEventData EventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ProductNameTag, EventData);
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

FName AHumanCharacter::GetLeftHandSocket()
{
	return LeftHandSocket;
}

FName AHumanCharacter::GetRightHandSocket()
{
	return RightHandSocket;
}

FName AHumanCharacter::GetMainHandSocket()
{
	if (bIsRightHanded)
	{
		return RightHandSocket;
	}
	else
	{
		return LeftHandSocket;
	}
}

FName AHumanCharacter::GetSecondaryHandSocket()
{
	if (bIsRightHanded)
	{
		return LeftHandSocket;
	}
	else
	{
		return RightHandSocket;
	}
}

UObject* AHumanCharacter::GetFocusedInteractableObject()
{
	return FocusedInteractableObject;
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

	if (FocusedInteractableObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject is a valid pointer"), *Prefix);

		IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(FocusedInteractableObject);
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

	if (IsValid(MainHandItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem is a valid pointer"), *Prefix);

		if (MainHandItem->bCanInteract(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem can be interacted with, THIS SHOULD NOT BE POSSIBLE"), *Prefix);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem cannot be interacted with as it is currently equiped by the player"), *Prefix);
		}
		if (MainHandItem->GetActorEnableCollision())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem has collisions enabled"), *Prefix);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem does not have collisions enabled"), *Prefix);
		}


		UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem is a %s"), *Prefix, *MainHandItem->ItemData.ItemName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s MainHandItem == nullptr"), *Prefix);
	}

	if (IsValid(SecondaryHandItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem is a valid pointer"), *Prefix);

		if (SecondaryHandItem->bCanInteract(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem can be interacted with, THIS SHOULD NOT BE POSSIBLE"), *Prefix);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem cannot be interacted with as it is currently equiped by the player"), *Prefix);
		}
		if (SecondaryHandItem->GetActorEnableCollision())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem has collisions enabled"), *Prefix);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem does not have collisions enabled"), *Prefix);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem is a %s"), *Prefix, *SecondaryHandItem->ItemData.ItemName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s SecondaryHandItem == nullptr"), *Prefix);
	}
}


