// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSets/SG_AttributeSet.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "Characters/InGameCharacter.h"
#include "Interfaces/HittableInterface.h"
#include "GAS/SG_AbilitySystemComponent.h"


void USG_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
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
		IHittableInterface* HittableAvatarActor = Cast<IHittableInterface>(GetOwningActor());
		if (HittableAvatarActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange AttributeOwner == nullptr"));
			return;
		}

		if (HasMaxHealth())
		{
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
		}
		else
		{
			ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
		}

		if (GetHealth() == 0.f && HittableAvatarActor->bCanBeHit())
		{
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNoHealth")));
			HittableAvatarActor->OnHittableDestroyed();
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
