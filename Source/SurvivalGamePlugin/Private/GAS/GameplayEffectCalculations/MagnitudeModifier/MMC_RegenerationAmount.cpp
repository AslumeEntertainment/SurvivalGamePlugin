// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/MagnitudeModifier/MMC_RegenerationAmount.h"

#include "Interactables/Items/RegeneratingItem.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "Characters/InGameCharacter.h"


UMMC_RegenerationAmount::UMMC_RegenerationAmount()
{
}

float UMMC_RegenerationAmount::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	ARegeneratingItem* RegeneratingItem = Cast<ARegeneratingItem>(Spec.GetContext().Get()->GetSourceObject());
	if (IsValid(RegeneratingItem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UMMC_RegenerationAmount::CalculateBaseMagnitude_Implementation IsValid(RegeneratingItem) == false"));
		return 0.0f;
	}

	float RegenerationValue = RegeneratingItem->GetRegenerationAmount();
	return RegenerationValue;
}


