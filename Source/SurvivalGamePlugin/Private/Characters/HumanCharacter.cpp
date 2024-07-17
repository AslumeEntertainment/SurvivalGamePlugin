// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/HumanCharacter.h"
#include "Interactables/InteractableInterface.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"


void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EmitInteractionChecker();
}

void AHumanCharacter::EmitInteractionChecker()
{
	FVector StartTrace = GetFollowCamera()->GetComponentLocation();
	FVector EndTrace = GetFollowCamera()->GetForwardVector() * InteractionDistance + StartTrace;

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

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(EquipMainHandAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptEquipWithMainHand);
		EnhancedInputComponent->BindAction(EquipSecondaryHandAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AttemptEquipWithSecondaryHand);
	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AHumanCharacter::AttemptEquipWithMainHand()
{
	if (IsValid(FocusedInteractableObject))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, TEXT("Attempting to equip with main hand"));
		Cast<AActor>(FocusedInteractableObject)->Destroy();
	}
}

void AHumanCharacter::AttemptEquipWithSecondaryHand()
{
	if (IsValid(FocusedInteractableObject))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, TEXT("Attempting to equip with secondary hand"));
		Cast<AActor>(FocusedInteractableObject)->Destroy();
	}
}
