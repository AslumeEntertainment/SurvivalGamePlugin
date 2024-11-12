// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSets/SG_AttributeSet.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "Characters/InGameCharacter.h"
#include "Components/HittableComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"


USG_AttributeSet::USG_AttributeSet()
{
	
}

void USG_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, IgnitionThreshold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, FireDamageResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, ExtinguishTime, COND_None, REPNOTIFY_Always);
}

void USG_AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHealth());
	}
}

void USG_AttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(GetOwningAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange IsValid(ASC) == false"));
		return;
	}

	if (Attribute == GetHealthAttribute())
	{
		UHittableComponent* HittableComp = GetOwningActor()->GetComponentByClass<UHittableComponent>();
		if (IsValid(HittableComp) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange IsValid(HittableComp) == false"));
			return;
		}

		if (HasMaxHealth())
		{
			ASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
		}
		else if(ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth"))))
		{
			ASC->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
			ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
		}

		if (GetHealth() == 0.f)
		{
			ASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNoHealth")));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNoHealth")));
		}
	}
}

void USG_AttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetMaxHealthAttribute())
	{
		ClampAttributeOnMaxValueChanged(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void USG_AttributeSet::ClampAttributeOnMaxValueChanged(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();

	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::ClampAttributeOnMaxValueChanged IsValid(ASC) == false"));
		return;
	}
	
	float CurrentValue = AffectedAttribute.GetCurrentValue();
	if (CurrentValue <= 0)
	{
		return;
	}
	
	CurrentValue = FMath::Clamp<float>(CurrentValue, 0, CurrentMaxValue);
	ASC->SetNumericAttributeBase(AffectedAttributeProperty, CurrentValue);
}

bool USG_AttributeSet::HasMaxHealth() const
{
	return GetHealth() == GetMaxHealth();
}

void USG_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Health, OldHealth);
}

void USG_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxHealth, OldMaxHealth);
}

void USG_AttributeSet::OnRep_IgnitionThreshold(const FGameplayAttributeData& OldIgnitionThreshold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, IgnitionThreshold, OldIgnitionThreshold);
}

void USG_AttributeSet::OnRep_FireDamageResistance(const FGameplayAttributeData& OldFireDamageResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, FireDamageResistance, OldFireDamageResistance);
}

void USG_AttributeSet::OnRep_ExtinguishTime(const FGameplayAttributeData& OldExtinguishTime)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, ExtinguishTime, OldExtinguishTime);
}
