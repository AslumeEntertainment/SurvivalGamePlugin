// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/InGameCharacter.h"

void AInGameCharacter::Die()
{
	Super::Die();

	OnCharacterDeath.Broadcast();

}