// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemBlueprintLibrary.h"

#include "Kismet/KismetMathLibrary.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"


//bool USG_AbilitySystemBlueprintLibrary::CalculateAndApplyImpactDamage(AActor* Actor, UAbilitySystemComponent* ASC)
//{
//	if (ASC == nullptr)
//	{
//		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemBlueprintLibrary::CalculateImpactDamage ASC == nullptr"));
//		return false;
//	}
//
//	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasInvincibility")) ||
//		ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasImmunity.ImpactDamage")))
//	{
//		return false;
//	}
//
//	float Damage = 0;
//	FVector Velocity = Actor->GetVelocity();
//	FString DamageContextString;
//
//	if (UKismetMathLibrary::Normal(Velocity).Z < -0.5)
//	{
//		Damage = FMath::Abs(Velocity.Z) * 0.08 - 60;
//		DamageContextString = "Fall";
//	}
//	else
//	{
//		Damage = Velocity.Length() * 0.057 - 71;
//		DamageContextString = "Impact";
//	}
//	
//	// Damage *= Actor.Vulnerability
//	Damage = FMath::Clamp<float>(Damage, 0, ASC->GetNumericAttribute(USG_AttributeSet::GetMaxHealthAttribute()));
//	Damage = FMath::CeilToFloat(Damage);
//
//	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Velocity on %s - %f"), *DamageContextString, Velocity.Length()));
//	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("%s damage - %f"), *DamageContextString, Damage));
//
//	if (Damage > 0.f)
//	{
//		ASC->ApplyModToAttribute(USG_AttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage);
//		return true;
//	}
//	return false;
//}

