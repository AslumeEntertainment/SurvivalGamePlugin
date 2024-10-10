// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "SG_CharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_CharacterAttributeSet : public USG_AttributeSet
{
	GENERATED_BODY()
	
public:

	USG_CharacterAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_HealthRegenerationRate)
	FGameplayAttributeData HealthRegenerationRate;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, HealthRegenerationRate);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, Attack);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Hunger)
	FGameplayAttributeData Hunger;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, Hunger);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHunger)
	FGameplayAttributeData MaxHunger;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, MaxHunger);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_HungerDrainRate)
	FGameplayAttributeData HungerDrainRate;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, HungerDrainRate);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, Energy);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, MaxEnergy);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_EnergyRechargeRate)
	FGameplayAttributeData EnergyRechargeRate;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, EnergyRechargeRate);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_EnergyDrainRate)
	FGameplayAttributeData EnergyDrainRate;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, EnergyDrainRate);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, Speed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_JumpVelocity)
	FGameplayAttributeData JumpVelocity;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, JumpVelocity);

	bool HasMaxHunger() const;
	bool HasMaxEnergy() const;

protected:

	UFUNCTION()
	virtual void OnRep_HealthRegenerationRate(const FGameplayAttributeData& OldHealthRegenerationRate);

	UFUNCTION()
	virtual void OnRep_Attack(const FGameplayAttributeData& OldAttack);

	UFUNCTION()
	virtual void OnRep_Hunger(const FGameplayAttributeData& OldHunger);

	UFUNCTION()
	virtual void OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger);

	UFUNCTION()
	virtual void OnRep_HungerDrainRate(const FGameplayAttributeData& OldHungerDrainRate);

	UFUNCTION()
	virtual void OnRep_Energy(const FGameplayAttributeData& OldEnergy);

	UFUNCTION()
	virtual void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy);

	UFUNCTION()
	virtual void OnRep_EnergyRechargeRate(const FGameplayAttributeData& OldEnergyRechargeRate);

	UFUNCTION()
	virtual void OnRep_EnergyDrainRate(const FGameplayAttributeData& OldEnergyDrainRate);

	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldSpeed);

	UFUNCTION()
	virtual void OnRep_JumpVelocity(const FGameplayAttributeData& OldJumpVelocity);

};
