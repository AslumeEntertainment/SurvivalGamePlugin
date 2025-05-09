// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSets/SG_AttributeSet.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/ItemBase.h"
#include "Characters/InGameCharacter.h"
#include "Interfaces/Hittable.h"


void USG_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxHealth, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, BaseHealth, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, CurrentGuardProficiency, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, CurrentMaxGuardProficiency, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxGuardProficiency, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, BaseGuardProficiency, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, CurrentSlashingDefence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxSlashingDefence, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, BaseSlashingDefence, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, CurrentPiercingDefence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxPiercingDefence, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, BasePiercingDefence, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, CurrentBludgeoningDefence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxBludgeoningDefence, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, BaseBludgeoningDefence, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, CurrentFireDefence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxFireDefence, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, BaseFireDefence, COND_None, REPNOTIFY_OnChanged);
}

void USG_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetBaseHealthAttribute())
	{
		float CurrentDifference = GetBaseHealth() == 0 ? 1 : (GetMaxHealth() / GetBaseHealth());
		float CurrentPercent = GetMaxHealth() == 0 ? 1 : (GetCurrentHealth() / GetMaxHealth());

		SetMaxHealth(NewValue * CurrentDifference);
		SetCurrentHealth(NewValue * CurrentPercent);
	}
	

	else if (Attribute == GetBaseGuardProficiencyAttribute())
	{
		float CurrentDifference = GetBaseGuardProficiency() == 0 ? 1 : (GetMaxGuardProficiency() / GetBaseGuardProficiency());
		SetMaxGuardProficiency(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxGuardProficiencyAttribute())
	{
		SetCurrentMaxGuardProficiency(NewValue);
	}
	else if (Attribute == GetCurrentMaxGuardProficiencyAttribute())
	{
		SetCurrentGuardProficiency(0);
	}


	else if (Attribute == GetBaseSlashingDefenceAttribute())
	{
		float CurrentDifference = GetBaseSlashingDefence() == 0 ? 1 : (GetMaxSlashingDefence() / GetBaseSlashingDefence());
		SetMaxSlashingDefence(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxSlashingDefenceAttribute())
	{
		SetCurrentSlashingDefence(NewValue);
	}


	else if (Attribute == GetBasePiercingDefenceAttribute())
	{
		float CurrentDifference = GetBasePiercingDefence() == 0 ? 1 : (GetMaxPiercingDefence() / GetBasePiercingDefence());
		SetMaxPiercingDefence(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxPiercingDefenceAttribute())
	{
		SetCurrentPiercingDefence(NewValue);
	}


	else if (Attribute == GetBaseBludgeoningDefenceAttribute())
	{
		float CurrentDifference = GetBaseBludgeoningDefence() == 0 ? 1 : (GetMaxBludgeoningDefence() / GetBaseBludgeoningDefence());
		SetMaxBludgeoningDefence(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxBludgeoningDefenceAttribute())
	{
		SetCurrentBludgeoningDefence(NewValue);
	}


	else if (Attribute == GetBaseFireDefenceAttribute())
	{
		float CurrentDifference = GetBaseFireDefence() == 0 ? 1 : (GetMaxFireDefence() / GetBaseFireDefence());
		SetMaxFireDefence(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxFireDefenceAttribute())
	{
		SetCurrentFireDefence(NewValue);
	}
}

void USG_AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHealth());
	}
}

void USG_AttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	/*USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(GetOwningAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange IsValid(ASC) == false"));
		return;
	}*/

	
}

void USG_AttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetMaxHealthAttribute())
	{
		ClampAttributeOnMaxValueChanged(CurrentHealth, MaxHealth, NewValue, GetCurrentHealthAttribute());
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

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		UE_LOG(LogTemp, Log, TEXT("USG_AttributeSet: %s now has %.2f out of %.2f health"), *GetOwningActor()->GetName(), GetCurrentHealth(), GetMaxHealth());

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

		if (GetCurrentHealth() == 0.f)
		{
			IHittable* HittableOwner = Cast<IHittable>(ASC->GetOwner());
			if (HittableOwner == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange HittableOwner == nullptr"));
				return;
			}
			/*if (IsValid(Cast<AItemBase>(HittableOwner)) && IsValid(Cast<AItemBase>(HittableOwner)->GetAttachParentActor()))
			{
				return;
			}*/
			HittableOwner->DestroyHittable();
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
	return GetCurrentHealth() == GetMaxHealth();
}

void USG_AttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, CurrentHealth, OldValue);
}

void USG_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxHealth, OldValue);
}

void USG_AttributeSet::OnRep_BaseHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, BaseHealth, OldValue);
}

void USG_AttributeSet::OnRep_CurrentGuardProficiency(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, CurrentGuardProficiency, OldValue);
}

void USG_AttributeSet::OnRep_CurrentMaxGuardProficiency(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, CurrentMaxGuardProficiency, OldValue);
}

void USG_AttributeSet::OnRep_MaxGuardProficiency(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxGuardProficiency, OldValue);
}

void USG_AttributeSet::OnRep_BaseGuardProficiency(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, BaseGuardProficiency, OldValue);
}

void USG_AttributeSet::OnRep_CurrentSlashingDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, CurrentSlashingDefence, OldValue);
}

void USG_AttributeSet::OnRep_MaxSlashingDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxSlashingDefence, OldValue);
}

void USG_AttributeSet::OnRep_BaseSlashingDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, BaseSlashingDefence, OldValue);
}

void USG_AttributeSet::OnRep_CurrentPiercingDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, CurrentPiercingDefence, OldValue);
}

void USG_AttributeSet::OnRep_MaxPiercingDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxPiercingDefence, OldValue);
}

void USG_AttributeSet::OnRep_BasePiercingDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, BasePiercingDefence, OldValue);
}

void USG_AttributeSet::OnRep_CurrentBludgeoningDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, CurrentBludgeoningDefence, OldValue);
}

void USG_AttributeSet::OnRep_MaxBludgeoningDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxBludgeoningDefence, OldValue);
}

void USG_AttributeSet::OnRep_BaseBludgeoningDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, BaseBludgeoningDefence, OldValue);
}

void USG_AttributeSet::OnRep_CurrenFireDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, CurrentFireDefence, OldValue);
}

void USG_AttributeSet::OnRep_MaxFireDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxFireDefence, OldValue);
}

void USG_AttributeSet::OnRep_BaseFireDefence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, BaseFireDefence, OldValue);
}
