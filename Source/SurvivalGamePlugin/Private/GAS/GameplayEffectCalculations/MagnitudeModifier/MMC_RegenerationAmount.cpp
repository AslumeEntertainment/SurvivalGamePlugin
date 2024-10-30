// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/MagnitudeModifier/MMC_RegenerationAmount.h"

#include "Objects/ConsumableItemEffectDataObject.h"

float UMMC_RegenerationAmount::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	UConsumableItemEffectDataObject* ConsumableItemEffectDataObject = Cast<UConsumableItemEffectDataObject>(Spec.GetContext().Get()->GetSourceObject());
	if (IsValid(ConsumableItemEffectDataObject) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UMMC_RegenerationAmount::CalculateBaseMagnitude_Implementation IsValid(ConsumableItemEffectDataObject) == false"));
		return 0.0f;
	}

	return ConsumableItemEffectDataObject->EffectValue;
}


