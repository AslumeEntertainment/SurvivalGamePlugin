// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemBlueprintLibrary.h"

#include "Kismet/KismetMathLibrary.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"


bool USG_AbilitySystemBlueprintLibrary::CalculateAndApplyImpactDamage(AActor* Actor, UAbilitySystemComponent* ASC)
{
	if (ASC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemBlueprintLibrary::CalculateImpactDamage ASC == nullptr"));
		return false;
	}

	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasInvincibility")) ||
		ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasImmunity.ImpactDamage")))
	{
		return false;
	}

	FVector Velocity = Actor->GetVelocity();
	float Damage = Velocity.Length() * 0.065 - 98;
	// Damage *= Actor.Vulnerability
	Damage = FMath::Clamp<float>(Damage, 0, ASC->GetNumericAttribute(USG_AttributeSet::GetMaxHealthAttribute()));
	Damage = FMath::CeilToFloat(Damage);

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Impact damage - %f"), Damage));

	if (Damage > 0.f)
	{
		ASC->ApplyModToAttribute(USG_AttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage);
		return true;
	}
	return false;
}

bool USG_AbilitySystemBlueprintLibrary::CalculateAndApplyFallDamage(AActor* Actor, UAbilitySystemComponent* ASC)
{
	if (ASC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemBlueprintLibrary::CalculateImpactDamage ASC == nullptr"));
		return false;
	}

	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasInvincibility")) || 
		ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasImmunity.FallDamage")))
	{
		return false;
	}

	FVector Velocity = Actor->GetVelocity();
	float Damage = FMath::Abs(Velocity.Z) * 0.08 - 60;
	// Damage *= Actor.Vulnerability
	Damage = FMath::Clamp<float>(Damage, 0, ASC->GetNumericAttribute(USG_AttributeSet::GetMaxHealthAttribute()));
	Damage = FMath::CeilToFloat(Damage);

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Fall damage - %f"), Damage));

	if (Damage > 0.f)
	{
		ASC->ApplyModToAttribute(USG_AttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage);
		return true;
	}
	return false;
}
