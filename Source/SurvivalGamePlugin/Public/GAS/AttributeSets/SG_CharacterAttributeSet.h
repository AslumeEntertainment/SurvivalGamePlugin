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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_HealthRegenerationRate)
	FGameplayAttributeData HealthRegenerationRate;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, HealthRegenerationRate);

	/*---------------------------------------------Combat-----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, Strength);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Dexterity)
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, Dexterity);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseAttackSpeedPercent)
	FGameplayAttributeData BaseAttackSpeedPercent;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, BaseAttackSpeedPercent);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentAttackSpeedPercent)
	FGameplayAttributeData CurrentAttackSpeedPercent;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, CurrentAttackSpeedPercent);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxAttackSpeedPercent)
	FGameplayAttributeData MaxAttackSpeedPercent;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, MaxAttackSpeedPercent);
	/*---------------------------------------------Hunger-----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Hunger)
	FGameplayAttributeData Hunger;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, Hunger);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHunger)
	FGameplayAttributeData MaxHunger;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, MaxHunger);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_HungerDrainRate)
	FGameplayAttributeData HungerDrainRate;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, HungerDrainRate);

	/*---------------------------------------------Energy-----------------------------------------------*/

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

	/*---------------------------------------------Movement-----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseSpeed)
	FGameplayAttributeData BaseSpeed;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, BaseSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentSpeed)
	FGameplayAttributeData CurrentSpeed;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, CurrentSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxSpeed)
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, MaxSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseJumpVelocity)
	FGameplayAttributeData BaseJumpVelocity;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, BaseJumpVelocity);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentJumpVelocity)
	FGameplayAttributeData CurrentJumpVelocity;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, CurrentJumpVelocity);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxJumpVelocity)
	FGameplayAttributeData MaxJumpVelocity;
	ATTRIBUTE_ACCESSORS(USG_CharacterAttributeSet, MaxJumpVelocity);

	bool HasMaxHunger() const;
	bool HasMaxEnergy() const;

protected:

	UFUNCTION()
	virtual void OnRep_HealthRegenerationRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Strength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Dexterity(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BaseAttackSpeedPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CurrentAttackSpeedPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxAttackSpeedPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Hunger(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHunger(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HungerDrainRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Energy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxEnergy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_EnergyRechargeRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_EnergyDrainRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BaseSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CurrentSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BaseJumpVelocity(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CurrentJumpVelocity(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxJumpVelocity(const FGameplayAttributeData& OldValue);

};
