// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_ManualDurabilityReduction.h"

#include "Interactables/Items/ItemBase.h"

void UGEEC_ManualDurabilityReduction::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FGameplayEffectContextHandle EffectContext = ExecutionParams.GetOwningSpec().GetEffectContext();

	if (IsValid(EffectContext.GetEffectCauser()) == false)
	{
		return;
	}

	AItemBase* Item = Cast<AItemBase>(EffectContext.GetEffectCauser());

	if (IsValid(Item) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_ManualDurabilityReduction::Execute_Implementation IsValid(Item) == false"));
		return;
	}
	Item->LoseDurability(Ammount);
}
