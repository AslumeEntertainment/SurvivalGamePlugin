// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Characters/SG_Character.h"
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

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, DamageRight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, DamageLeft, COND_None, REPNOTIFY_Always);

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

void USG_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();

	ASG_Character* TargetCharacter;
	if (Data.Target.AbilityActorInfo.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostGameplayEffectExecute Data.Target.AbilityActorInfo.IsValid() == false"));
		return;
	}
	if (Data.Target.AbilityActorInfo->AvatarActor.IsValid() == false)
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
		TargetCharacter->NetMulticast_OnUpdatePlayerHealth(GetHealth(), GetMaxHealth());
		if (GetHealth() == 0.f)
		{
			TargetCharacter->Die();
		}
	}

	if (Data.EvaluatedData.Attribute == GetHungerAttribute())
	{
		if (HasMaxHunger())
		{
			TargetCharacter->GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHunger")));
		}
		else
		{
			TargetCharacter->GetAbilitySystemComponent()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHunger")));
		}
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

void USG_AttributeSet::OnRep_DamageRight(const FGameplayAttributeData& OldDamageRight)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, DamageRight, OldDamageRight);
}

void USG_AttributeSet::OnRep_DamageLeft(const FGameplayAttributeData& OldDamageLeft)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, DamageLeft, OldDamageLeft);
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

