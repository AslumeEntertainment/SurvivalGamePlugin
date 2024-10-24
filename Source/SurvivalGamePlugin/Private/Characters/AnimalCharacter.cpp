// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AnimalCharacter.h"

#include "Components/DamageDealingComponent.h"


AAnimalCharacter::AAnimalCharacter()
{
	MouthDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("MouthDamageOrigin"));
	MouthDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("head"));
}


