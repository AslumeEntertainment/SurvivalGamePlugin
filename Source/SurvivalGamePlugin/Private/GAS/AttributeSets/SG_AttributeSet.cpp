// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSets/SG_AttributeSet.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "FunctionLibraries/DamageFunctionLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Components/HittableComponent.h"
#include "Characters/InGameCharacter.h"


USG_AttributeSet::USG_AttributeSet()
{
	
}

void USG_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, ResistanceSlashing, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, ResistancePiercing, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, ResistanceBludgeoning, COND_None, REPNOTIFY_Always);

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
	//keeping this method just in case
}

void USG_AttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetMaxHealthAttribute())
	{
		ClampAttributeOnMaxValueChanged(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void USG_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(GetOwningAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostGameplayEffectExecute IsValid(ASC) == false"));
		return;
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UHittableComponent* HittableComp = GetOwningActor()->GetComponentByClass<UHittableComponent>();
		if (IsValid(HittableComp) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostGameplayEffectExecute IsValid(HittableComp) == false"));
			return;
		}

		if (Data.EvaluatedData.Magnitude <= 0 && HittableComp->CanDisplayDamageIndicator())
		{
			UWorld* World = GetWorld();
			if (IsValid(World) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostGameplayEffectExecute IsValid(World) == false"));
				return;
			}
			UDamageFunctionLibrary::ShowDamageIndicator(World, FMath::Abs(Data.EvaluatedData.Magnitude), GetOwningActor());
		}

		UE_LOG(LogTemp, Log, TEXT("USG_AttributeSet: %s now has %.2f out of %.2f health"), *GetOwningActor()->GetName(), GetHealth(), GetMaxHealth());

		if (HasMaxHealth())
		{
			ASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Health")));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Health")));
		}
		else if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Health"))))
		{
			ASC->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Health")));
			ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Health")));
		}
		if (GetHealth() == 0.f)
		{
			ASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Health")));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Health")));
			HittableComp->OnHittableDestroyed();
		}
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

void USG_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Health, OldValue);
}

void USG_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxHealth, OldValue);
}

void USG_AttributeSet::OnRep_ResistanceSlashing(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, ResistanceSlashing, OldValue);
}

void USG_AttributeSet::OnRep_ResistancePiercing(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, ResistancePiercing, OldValue);
}

void USG_AttributeSet::OnRep_ResistanceBludgeoning(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, ResistanceBludgeoning, OldValue);
}

void USG_AttributeSet::OnRep_IgnitionThreshold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, IgnitionThreshold, OldValue);
}

void USG_AttributeSet::OnRep_FireDamageResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, FireDamageResistance, OldValue);
}

void USG_AttributeSet::OnRep_ExtinguishTime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, ExtinguishTime, OldValue);
}
