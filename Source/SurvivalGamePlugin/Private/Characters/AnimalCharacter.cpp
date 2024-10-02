// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AnimalCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Components/DamageDealingComponent.h"


AAnimalCharacter::AAnimalCharacter()
{
	MouthDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("MouthDamageOrigin"));
	MouthDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("head"));
}

void AAnimalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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

