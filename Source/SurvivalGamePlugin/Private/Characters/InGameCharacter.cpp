// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/InGameCharacter.h"
#include "Components/DamageDealingComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_CharacterAbilitySystemComponent.h"
#include "GAS/SG_GameplayEffectContext.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"

//void AInGameCharacter::Die()
//{
//	Super::Die();
//
//	OnCharacterDeath.Broadcast();
//
//}

AInGameCharacter::AInGameCharacter()
{
	LeftHandDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("LeftHandDamageOrigin"));
	LeftHandDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("hand_l"));

	RightHandDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("RightHandDamageOrigin"));
	RightHandDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("hand_r"));
}

void AInGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AInGameCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AInGameCharacter::FinishSprint);
	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AInGameCharacter::StartSprint()
{
	SendLocalInputToAbilitySystemComponent(true, EGameplayAbilityInputID::Sprint);
}

void AInGameCharacter::FinishSprint()
{
	SendLocalInputToAbilitySystemComponent(false, EGameplayAbilityInputID::Sprint);
}

void AInGameCharacter::OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles)
{
	for (FGameplayEffectSpecHandle SpecHandle : GameplayEffectSpecHandles)
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

bool AInGameCharacter::bCanBeHit()
{
	return IsAlive();
}

void AInGameCharacter::OnHittableDestroyed_Implementation()
{
	bIsAlive = false;

	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	UnPossessed();
}

void AInGameCharacter::Client_OnUpdatePlayerHealth_Implementation(float CurrentHealth, float MaxHealth)
{
	OnHealthPercentUpdated.Broadcast(CurrentHealth / MaxHealth);
}

void AInGameCharacter::Client_OnUpdatePlayerHunger_Implementation(float CurrentHunger, float MaxHunger)
{
	OnHungerPercentUpdated.Broadcast(CurrentHunger / MaxHunger);
}

void AInGameCharacter::Client_OnUpdatePlayerEnergy_Implementation(float CurrentEnergy, float MaxEnergy)
{
	OnEnergyPercentUpdated.Broadcast(CurrentEnergy / MaxEnergy);
}

bool AInGameCharacter::IsAlive()
{
	return bIsAlive;
}
