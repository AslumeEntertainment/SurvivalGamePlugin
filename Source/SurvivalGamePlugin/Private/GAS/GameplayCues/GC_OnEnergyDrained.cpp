// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_OnEnergyDrained.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/Abilities/SGGA_OnEnergyDrained.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"

bool UGC_OnEnergyDrained::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	AInGameCharacter* TargetCharacter = Cast<AInGameCharacter>(MyTarget);
	if (IsValid(TargetCharacter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnEnergyDrained::OnExecute_Implementation IsValid(TargetCharacter) == false"));
		return false;
	}
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(TargetCharacter->GetAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_OnEnergyDrained::OnExecute_Implementation IsValid(ASC) == false"));
		return false;
	}

	if (ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetEnergyAttribute()) > 0) return false;

	ASC->AddPassiveAbility(OnEnergyDrainedPassiveAbility, 1, nullptr);

	return true;
}
