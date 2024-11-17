// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Components/DamageDealingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"


void USG_CharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, HealthRegenerationRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxHunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, HungerDrainRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, EnergyRechargeRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, EnergyDrainRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, JumpVelocity, COND_None, REPNOTIFY_Always);
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

	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(GetOwningAbilitySystemComponent());
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostAttributeChange IsValid(ASC) == false"));
		return;
	}
	AInGameCharacter* AvatarActor = Cast<AInGameCharacter>(ASC->GetAvatarActor());
	if (IsValid(AvatarActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostAttributeChange IsValid(AvatarActor) == false"));
		return;
	}

	if (Attribute == GetAttackAttribute())
	{
		TArray<UDamageDealingComponent*> PlayerDamageDealingComponents;
		AvatarActor->GetComponents<UDamageDealingComponent>(PlayerDamageDealingComponents, true);

		for (UDamageDealingComponent* DamageDealingComponent : PlayerDamageDealingComponents)
		{
			DamageDealingComponent->SetDefaultDamageValue(NewValue);
		}
	}
	if (Attribute == GetAttackSpeedAttribute())
	{
		AvatarActor->SetAttackSpeed(NewValue);
	}
	if (Attribute == GetMaxHungerAttribute())
	{
		ClampAttributeOnMaxValueChanged(Hunger, MaxHunger, NewValue, GetHungerAttribute());
	}
	if (Attribute == GetEnergyAttribute())
	{
		if (NewValue == GetMaxEnergy())
		{
			AvatarActor->SendGameplayEventToSelf(FGameplayTag::RequestGameplayTag("Bool.HasFull.Energy"), FGameplayEventData());
		}
		else if (NewValue == 0.f)
		{
			ASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Energy")));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Energy")));
		}
	}
	if (Attribute == GetMaxEnergyAttribute())
	{
		ClampAttributeOnMaxValueChanged(Energy, MaxEnergy, NewValue, GetEnergyAttribute());
	}
	if (Attribute == GetSpeedAttribute())
	{
		UCharacterMovementComponent* MovementComp = AvatarActor->GetCharacterMovement();
		if (IsValid(MovementComp) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostAttributeChange IsValid(MovementComp) == false"));
			return;
		}
		MovementComp->MaxWalkSpeed = NewValue;
		MovementComp->MaxWalkSpeedCrouched = NewValue / 2;
	}
	if (Attribute == GetJumpVelocityAttribute())
	{
		UCharacterMovementComponent* MovementComp = AvatarActor->GetCharacterMovement();
		if (IsValid(MovementComp) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostAttributeChange IsValid(MovementComp) == false"));
			return;
		}
		MovementComp->JumpZVelocity = NewValue;
	}
}

void USG_CharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	UAbilitySystemComponent* Target = &(Data.Target);

	AInGameCharacter* TargetCharacter;
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

	TWeakObjectPtr<UAbilitySystemComponent> ASC = Target->AbilityActorInfo->AbilitySystemComponent;
	if (ASC.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostGameplayEffectExecute ASC.IsValid() == false"));
		return;
	}

	TargetCharacter = Cast<AInGameCharacter>(Data.Target.AbilityActorInfo->AvatarActor.Get());
	if (!IsValid(TargetCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostGameplayEffectExecute IsValid(TargetCharacter) == false"));
		return;
	}

	if (Data.EvaluatedData.Attribute == GetHungerAttribute())
	{
		if (GetHunger() >= GetMaxHunger() * 0.99)
		{
			ASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Hunger")));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Hunger")));
			ASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.CanRegenerateHealth")));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.CanRegenerateHealth")));
		}
		else 
		{
			if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasFull.Hunger")))
			{
				ASC->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Hunger")));
				ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFull.Hunger")));
			}
			if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.CurrentlyHealing")) == false && 
				ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.CanRegenerateHealth")))
			{
				ASC->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.CanRegenerateHealth")));
				ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.CanRegenerateHealth")));
			}
		}
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

void USG_CharacterAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, AttackSpeed, OldAttackSpeed);
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

void USG_CharacterAttributeSet::OnRep_JumpVelocity(const FGameplayAttributeData& OldJumpVelocity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, JumpVelocity, OldJumpVelocity);
}

