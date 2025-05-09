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

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, CurrentStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxStrength, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, BaseStrength, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, CurrentDexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxDexterity, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, BaseDexterity, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, BaseAttackSpeedPercent, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, CurrentAttackSpeedPercent, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxAttackSpeedPercent, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxHunger, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, HungerDrainRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxEnergy, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, EnergyRechargeRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, EnergyDrainRate, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, BaseSpeed, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, CurrentSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxSpeed, COND_None, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, BaseJumpVelocity, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, CurrentJumpVelocity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_CharacterAttributeSet, MaxJumpVelocity, COND_None, REPNOTIFY_OnChanged);
}

void USG_CharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetBaseStrengthAttribute())
	{
		float CurrentDifference = GetBaseStrength()==0 ? 1 : (GetMaxStrength() / GetBaseStrength());
		SetMaxStrength(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxStrengthAttribute())
	{
		SetCurrentStrength(NewValue);
	}

	else if (Attribute == GetBaseDexterityAttribute())
	{
		float CurrentDifference = GetBaseDexterity() == 0 ? 1 : (GetMaxDexterity() / GetBaseDexterity());
		SetMaxDexterity(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxDexterityAttribute())
	{
		SetCurrentDexterity(NewValue);
	}

	else if (Attribute == GetBaseAttackSpeedPercentAttribute())
	{
		float CurrentDifference = GetBaseAttackSpeedPercent() == 0 ? 1 : (GetMaxAttackSpeedPercent() / GetBaseAttackSpeedPercent());

		NewValue = FMath::Clamp<float>(NewValue * CurrentDifference, 0, 450);
		SetMaxAttackSpeedPercent(NewValue);
	}
	else if (Attribute == GetMaxAttackSpeedPercentAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, 450);

		SetCurrentAttackSpeedPercent(NewValue);
	}

	else if (Attribute == GetBaseSpeedAttribute())
	{
		float CurrentDifference = GetBaseSpeed() == 0 ? 1 : (GetMaxSpeed() / GetBaseSpeed());
		SetMaxSpeed(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxSpeedAttribute())
	{
		SetCurrentSpeed(NewValue);
	}

	else if(Attribute == GetBaseJumpVelocityAttribute())
	{
		float CurrentDifference = GetBaseJumpVelocity() == 0 ? 1 : (GetMaxJumpVelocity() / GetBaseJumpVelocity());
		SetMaxJumpVelocity(NewValue * CurrentDifference);
	}
	else if (Attribute == GetMaxJumpVelocityAttribute())
	{
		SetCurrentJumpVelocity(NewValue);
	}
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
		
		if (NewValue == 0.f)
		{
			ASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Energy")));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Energy")));
		}
		else if(ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Energy"))))
		{
			ASC->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Energy")));
			ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNone.Energy")));
		}
		
	}
	if (Attribute == GetMaxEnergyAttribute())
	{
		ClampAttributeOnMaxValueChanged(Energy, MaxEnergy, NewValue, GetEnergyAttribute());
	}
	if (Attribute == GetCurrentSpeedAttribute())
	{
		UCharacterMovementComponent* MovementComp = AvatarActor->GetCharacterMovement();
		if (IsValid(MovementComp) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("USG_CharacterAttributeSet::PostAttributeChange IsValid(MovementComp) == false"));
			return;
		}
		MovementComp->MaxAcceleration = NewValue * 4;
		MovementComp->MaxWalkSpeed = NewValue;
		MovementComp->MaxWalkSpeedCrouched = NewValue / 2;
	}
	if (Attribute == GetCurrentJumpVelocityAttribute())
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

void USG_CharacterAttributeSet::OnRep_HealthRegenerationRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, HealthRegenerationRate, OldValue);
}

void USG_CharacterAttributeSet::OnRep_CurrentStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, CurrentStrength, OldValue);
}

void USG_CharacterAttributeSet::OnRep_MaxStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxStrength, OldValue);
}

void USG_CharacterAttributeSet::OnRep_BaseStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, BaseStrength, OldValue);
}

void USG_CharacterAttributeSet::OnRep_CurrentDexterity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, CurrentDexterity, OldValue);
}

void USG_CharacterAttributeSet::OnRep_MaxDexterity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxDexterity, OldValue);
}

void USG_CharacterAttributeSet::OnRep_BaseDexterity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, BaseDexterity, OldValue);
}

void USG_CharacterAttributeSet::OnRep_BaseAttackSpeedPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, BaseAttackSpeedPercent, OldValue);
}

void USG_CharacterAttributeSet::OnRep_CurrentAttackSpeedPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, CurrentAttackSpeedPercent, OldValue);
}

void USG_CharacterAttributeSet::OnRep_MaxAttackSpeedPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxAttackSpeedPercent, OldValue);
}

void USG_CharacterAttributeSet::OnRep_Hunger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, Hunger, OldValue);
}

void USG_CharacterAttributeSet::OnRep_MaxHunger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxHunger, OldValue);
}

void USG_CharacterAttributeSet::OnRep_HungerDrainRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, HungerDrainRate, OldValue);
}

void USG_CharacterAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, Energy, OldValue);
}

void USG_CharacterAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxEnergy, OldValue);
}

void USG_CharacterAttributeSet::OnRep_EnergyRechargeRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, EnergyRechargeRate, OldValue);
}

void USG_CharacterAttributeSet::OnRep_EnergyDrainRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, EnergyDrainRate, OldValue);
}

void USG_CharacterAttributeSet::OnRep_BaseSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, BaseSpeed, OldValue);
}

void USG_CharacterAttributeSet::OnRep_CurrentSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, CurrentSpeed, OldValue);
}

void USG_CharacterAttributeSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxSpeed, OldValue);
}

void USG_CharacterAttributeSet::OnRep_BaseJumpVelocity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, BaseJumpVelocity, OldValue);
}

void USG_CharacterAttributeSet::OnRep_CurrentJumpVelocity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, CurrentJumpVelocity, OldValue);
}

void USG_CharacterAttributeSet::OnRep_MaxJumpVelocity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_CharacterAttributeSet, MaxJumpVelocity, OldValue);
}

