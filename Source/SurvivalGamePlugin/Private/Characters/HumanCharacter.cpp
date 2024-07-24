// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/HumanCharacter.h"
#include "Interactables/InteractableInterface.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Interactables/Items/ItemBase.h"
#include "UI/HUD/InGameHUD.h"

#include "AbilitySystemBlueprintLibrary.h"


void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (bIsRightHanded)
	{
		MainHandSocket = FName("Hand_R_Socket");
		SecondaryHandSocket = FName("Hand_L_Socket");
	}
	else
	{
		MainHandSocket = FName("Hand_L_Socket");
		SecondaryHandSocket = FName("Hand_R_Socket");
	}
}

void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EmitInteractionChecker();
}

void AHumanCharacter::EmitInteractionChecker()
{
	FVector StartTrace = GetFollowCamera()->GetComponentLocation();
	FVector EndTrace = GetFollowCamera()->GetForwardVector() * InteractionRange + StartTrace;

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

	World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_GameTraceChannel1, CQP);

	CheckForInteractables(HitResult);
}

void AHumanCharacter::CheckForInteractables(FHitResult HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	UPrimitiveComponent* HitComponent = HitResult.GetComponent();
	IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(HitActor);
	IInteractableInterface* InteractableComponent = Cast<IInteractableInterface>(HitComponent);

	if (HitActor == nullptr)
	{
		if (FocusedInteractableObject && Cast<AActor>(FocusedInteractableObject)->GetComponentByClass(UMeshComponent::StaticClass()))
		{
			UActorComponent* InteractableMeshComponent = Cast<AActor>(FocusedInteractableObject)->GetComponentByClass(UMeshComponent::StaticClass());
			if (Cast<UMeshComponent>(InteractableMeshComponent) && GetController()->IsLocalPlayerController())
			{
				Cast<UMeshComponent>(InteractableMeshComponent)->SetOverlayMaterial(nullptr);
			}
		}

		FocusedInteractableObject = nullptr;
		return;
	}
	if (InteractableComponent != nullptr)
	{
		FocusedInteractableObject = HitComponent;
		return;
	}
	if (InteractableActor == nullptr)
	{
		if (FocusedInteractableObject && Cast<AActor>(FocusedInteractableObject)->GetComponentByClass(UMeshComponent::StaticClass()))
		{
			UActorComponent* InteractableMeshComponent = Cast<AActor>(FocusedInteractableObject)->GetComponentByClass(UMeshComponent::StaticClass());
			if (Cast<UMeshComponent>(InteractableMeshComponent) && GetController()->IsLocalPlayerController())
			{
				Cast<UMeshComponent>(InteractableMeshComponent)->SetOverlayMaterial(nullptr);
			}
		}

		FocusedInteractableObject = nullptr;
		return;
	}
	if (!InteractableActor->bCanInteract())
	{
		return;
	}
	UMeshComponent* HitMeshComponent = Cast<UMeshComponent>(HitComponent);
	if (HitMeshComponent != nullptr && GetController()->IsLocalPlayerController())
	{
		HitMeshComponent->SetOverlayMaterial(InteractableOverlayMaterial);
	}

	FocusedInteractableObject = HitActor;
}

void AHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(EquipMainHandAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptEquipWithMainHand);
		EnhancedInputComponent->BindAction(EquipSecondaryHandAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptEquipWithSecondaryHand);
		
		EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptUseMainHandItem);
		EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptUseSecondaryHandItem);
		
		EnhancedInputComponent->BindAction(AbilityAction, ETriggerEvent::Triggered, this, &AHumanCharacter::UseAbility);

		EnhancedInputComponent->BindAction(CraftingAction, ETriggerEvent::Triggered, this, &AHumanCharacter::OpenCraftingWindow);
	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AHumanCharacter::AttemptEquipWithMainHand()
{
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::EquipMain);
}

void AHumanCharacter::AttemptEquipWithSecondaryHand()
{
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::EquipSecondary);
}

void AHumanCharacter::AttemptUseMainHandItem()
{
	if (IsValid(MainHandItem))
	{
		SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::UseMain);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, TEXT("Main hand is empty, performing another action"));
	}
}

void AHumanCharacter::AttemptUseSecondaryHandItem()
{
	if (IsValid(SecondaryHandItem))
	{
		SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::UseSecondary);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, TEXT("Secondary hand is empty, performing another action"));
	}
}

void AHumanCharacter::UseAbility()
{
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::Action);
}

void AHumanCharacter::OpenCraftingWindow()
{
	//Check if the crafting window is being opened
	if (bIsCraftingWindowOpen)
	{
		CraftingWindowCloseDelegate.Broadcast();
		bIsCraftingWindowOpen = false;
		return;
	}

	//Check if player has 2 items equiped
	if (!IsValid(MainHandItem) || !IsValid(SecondaryHandItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Player needs to have an item in both hands!"));
		return;
	}

	//Check if the player can craft using their equiped items
	FItemDataStruct* MainItemData = MainHandItem->ItemData.GetRow<FItemDataStruct>(TEXT("AHumanCharacter::OpenCraftingWindow: Retrieving main hand item data"));
	if (!MainItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("AHumanCharacter::OpenCraftingWindow MainItemData == nullptr"));
		return;
	}
	if (!MainItemData->bCanCraftWithItem(SecondaryHandItem->ItemData.RowName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Crafting: Player cannot craft anything with their current items!"));
		return;
	}
	
	CraftingWindowOpenDelegate.Broadcast(MainHandItem->ItemData, SecondaryHandItem->ItemData.RowName);
	bIsCraftingWindowOpen = true;
}

UObject* AHumanCharacter::GetFocusedInteractableObject()
{
	return FocusedInteractableObject;
}

FName AHumanCharacter::GetMainHandSocket()
{
	return MainHandSocket;
}

FName AHumanCharacter::GetSecondaryHandSocket()
{
	return SecondaryHandSocket;
}

void AHumanCharacter::CraftItem(FText ProductItemName)
{
	CraftingWindowCloseDelegate.Broadcast();
	bIsCraftingWindowOpen = false;

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
	EventData.EventTag = ProductNameTag;
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
