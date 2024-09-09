// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Characters/SG_Character.h"
#include "Components/DamageDealingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


USG_CharacterAttributeSet::USG_CharacterAttributeSet()
{
}

void USG_CharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, HealthRegenerationRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Attack, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxHunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, HungerDrainRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, EnergyRechargeRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, EnergyDrainRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Speed, COND_None, REPNOTIFY_Always);
}

void USG_CharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHungerAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHunger());
	}
	if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxEnergy());
	}
}

void USG_CharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetSpeedAttribute())
	{
		ASG_Character* AttributeOwner = Cast<ASG_Character>(GetOwningActor());
		if (!IsValid(AttributeOwner))
		{
			UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostAttributeBaseChange IsValid(AttributeOwner) == false"));
			return;
		}

		AttributeOwner->GetCharacterMovement()->MaxWalkSpeed = NewValue;
	}

	if (Attribute == GetAttackAttribute())
	{
		ASG_Character* AttributeOwner = Cast<ASG_Character>(GetOwningActor());
		if (!IsValid(AttributeOwner))
		{
			UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostAttributeBaseChange IsValid(AttributeOwner) == false"));
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

void USG_CharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	UAbilitySystemComponent* Target = &(Data.Target);

	ASG_Character* TargetCharacter;
	if (Target->AbilityActorInfo.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostGameplayEffectExecute Data.Target.AbilityActorInfo.IsValid() == false"));
		return;
	}
	if (Target->AbilityActorInfo->AvatarActor.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostGameplayEffectExecute Data.Target.AbilityActorInfo->AvatarActor.IsValid() == false"));
		return;
	}

	TargetCharacter = Cast<ASG_Character>(Data.Target.AbilityActorInfo->AvatarActor.Get());
	if (!IsValid(TargetCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostGameplayEffectExecute IsValid(TargetCharacter) == false"));
		return;
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

bool USG_CharacterAttributeSet::HasMaxHunger() const
{
	return GetHunger() == GetMaxHunger();
}

bool USG_CharacterAttributeSet::HasMaxEnergy() const
{
	return GetEnergy() == GetMaxEnergy();
}

void USG_CharacterAttributeSet::OnRep_HealthRegenerationRate(const FGameplayAttributeData& OldHealthRegenerationRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, HealthRegenerationRate, OldHealthRegenerationRate);
}

void USG_CharacterAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, Attack, OldAttack);
}

void USG_CharacterAttributeSet::OnRep_Hunger(const FGameplayAttributeData& OldHunger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, Hunger, OldHunger);
}

void USG_CharacterAttributeSet::OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxHunger, OldMaxHunger);
}

void USG_CharacterAttributeSet::OnRep_HungerDrainRate(const FGameplayAttributeData& OldHungerDrainRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, HungerDrainRate, OldHungerDrainRate);
}

void USG_CharacterAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, Energy, OldEnergy);
}

void USG_CharacterAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxEnergy, OldMaxEnergy);
}

void USG_CharacterAttributeSet::OnRep_EnergyRechargeRate(const FGameplayAttributeData& OldEnergyRechargeRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, EnergyRechargeRate, OldEnergyRechargeRate);
}

void USG_CharacterAttributeSet::OnRep_EnergyDrainRate(const FGameplayAttributeData& OldEnergyDrainRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, EnergyDrainRate, OldEnergyDrainRate);
}

void USG_CharacterAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, Speed, OldSpeed);
}

