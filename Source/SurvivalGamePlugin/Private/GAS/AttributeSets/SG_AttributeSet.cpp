// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Characters/SG_Character.h"
#include "Components/DamageDealingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

USG_AttributeSet::USG_AttributeSet()
{
}

void USG_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, HealthRegenerationRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Attack, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxHunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, HungerDrainRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, EnergyRechargeRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, EnergyDrainRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Speed, COND_None, REPNOTIFY_Always);
}

void USG_AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHealth());
	}
	if (Attribute == GetHungerAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHunger());
	}
	if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxEnergy());
	}
}

void USG_AttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetSpeedAttribute())
	{
		ASG_Character* AttributeOwner = Cast<ASG_Character>(GetOwningActor());
		if (!IsValid(AttributeOwner))
		{
			UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange IsValid(AttributeOwner) == false"));
			return;
		}

		AttributeOwner->GetCharacterMovement()->MaxWalkSpeed = NewValue;
	}

	if (Attribute == GetAttackAttribute())
	{
		ASG_Character* AttributeOwner = Cast<ASG_Character>(GetOwningActor());
		if (!IsValid(AttributeOwner))
		{
			UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange IsValid(AttributeOwner) == false"));
			return;
		}

		TArray<UDamageDealingComponent*> PlayerDamageDealingComponents;
		AttributeOwner->GetComponents<UDamageDealingComponent>(PlayerDamageDealingComponents, true);

		for (UDamageDealingComponent* DamageDealingComponent : PlayerDamageDealingComponents)
		{
			DamageDealingComponent->SetDefaultDamageValue(NewValue);
		}
	}
}

void USG_AttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	/* Functionality was moved to USG_AttributeSet::PostAttributeChange */
}

void USG_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	UAbilitySystemComponent* Target = &(Data.Target);

	ASG_Character* TargetCharacter;
	if (Target->AbilityActorInfo.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostGameplayEffectExecute Data.Target.AbilityActorInfo.IsValid() == false"));
		return;
	}
	if (Target->AbilityActorInfo->AvatarActor.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostGameplayEffectExecute Data.Target.AbilityActorInfo->AvatarActor.IsValid() == false"));
		return;
	}

	TargetCharacter = Cast<ASG_Character>(Data.Target.AbilityActorInfo->AvatarActor.Get());
	if (!IsValid(TargetCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostGameplayEffectExecute IsValid(TargetCharacter) == false"));
		return;
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{	
		if (HasMaxHealth())
		{
			TargetCharacter->GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
		}
		else
		{
			TargetCharacter->GetAbilitySystemComponent()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
		}

		TargetCharacter->OnUpdatePlayerHealth(GetHealth(), GetMaxHealth());
		if (GetHealth() == 0.f)
		{
			TargetCharacter->Die();
		}
	}

	if (Data.EvaluatedData.Attribute == GetHungerAttribute())
	{
		if (GetHunger() <= GetMaxHunger() / 2)
		{
			TargetCharacter->OnUpdatePlayerHunger(GetHunger(), GetMaxHunger());
		}
		
		if (HasMaxHunger())
		{
			TargetCharacter->GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHunger")));
		}
		else
		{
			TargetCharacter->GetAbilitySystemComponent()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHunger")));
		}
	}

	if (Data.EvaluatedData.Attribute == GetEnergyAttribute() && GetEnergy() < GetMaxEnergy() / 2)
	{
		TargetCharacter->OnUpdatePlayerEnergy(GetEnergy(), GetMaxEnergy() / 2);
	}
}

bool USG_AttributeSet::HasMaxHealth()
{
	return GetHealth() == GetMaxHealth();
}

bool USG_AttributeSet::HasMaxHunger()
{
	return GetHunger() == GetMaxHunger();
}

bool USG_AttributeSet::HasMaxEnergy()
{
	return GetEnergy() == GetMaxEnergy();
}

void USG_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Health, OldHealth);
}

void USG_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxHealth, OldMaxHealth);
}

void USG_AttributeSet::OnRep_HealthRegenerationRate(const FGameplayAttributeData& OldHealthRegenerationRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, HealthRegenerationRate, OldHealthRegenerationRate);
}

void USG_AttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Attack, OldAttack);
}

void USG_AttributeSet::OnRep_Hunger(const FGameplayAttributeData& OldHunger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Hunger, OldHunger);
}

void USG_AttributeSet::OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxHunger, OldMaxHunger);
}

void USG_AttributeSet::OnRep_HungerDrainRate(const FGameplayAttributeData& OldHungerDrainRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, HungerDrainRate, OldHungerDrainRate);
}

void USG_AttributeSet::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Energy, OldEnergy);
}

void USG_AttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxEnergy, OldMaxEnergy);
}

void USG_AttributeSet::OnRep_EnergyRechargeRate(const FGameplayAttributeData& OldEnergyRechargeRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, EnergyRechargeRate, OldEnergyRechargeRate);
}

void USG_AttributeSet::OnRep_EnergyDrainRate(const FGameplayAttributeData& OldEnergyDrainRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, EnergyDrainRate, OldEnergyDrainRate);
}

void USG_AttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Speed, OldSpeed);
}

