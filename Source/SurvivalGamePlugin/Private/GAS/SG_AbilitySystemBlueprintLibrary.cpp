// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemBlueprintLibrary.h"

#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"

void USG_AbilitySystemBlueprintLibrary::CalculateAndApplyImpactDamage(AActor* Actor, const float Velocity)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent(Actor);
	if (ASC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemBlueprintLibrary::CalculateImpactDamage ASC == nullptr"))
		return;
	}

	float Damage = Velocity * 0.08 - 60;
	// Damage *= Actor.Vulnerability
	Damage = FMath::Clamp<float>(Damage, 0, ASC->GetNumericAttribute(USG_AttributeSet::GetMaxHealthAttribute()));
	Damage = FMath::CeilToFloat(Damage);

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Impact damage - %f"), Damage));

	ASC->ApplyModToAttribute(USG_AttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage);
	return;
}
