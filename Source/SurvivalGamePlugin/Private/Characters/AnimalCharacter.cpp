// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AnimalCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AAnimalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(AnimalBiteAction, ETriggerEvent::Triggered, this, &AAnimalCharacter::AttemptBite);
		EnhancedInputComponent->BindAction(AnimalScratchAction, ETriggerEvent::Triggered, this, &AAnimalCharacter::AttemptScratch);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AAnimalCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AAnimalCharacter::AttemptBite()
{
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::AnimalBite);
}

void AAnimalCharacter::AttemptScratch()
{
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::AnimalScratch);
}