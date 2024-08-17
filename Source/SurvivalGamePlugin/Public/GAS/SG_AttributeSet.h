// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SG_AttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	USG_AttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_HealthRegenerationRate)
	FGameplayAttributeData HealthRegenerationRate;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, HealthRegenerationRate);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_DamageRight)
	FGameplayAttributeData DamageRight;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, DamageRight);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_DamageLeft)
	FGameplayAttributeData DamageLeft;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, DamageLeft);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Hunger)
	FGameplayAttributeData Hunger;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, Hunger);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHunger)
	FGameplayAttributeData MaxHunger;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxHunger);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_HungerDrainRate)
	FGameplayAttributeData HungerDrainRate;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, HungerDrainRate);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, Energy);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxEnergy);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_EnergyRechargeRate)
	FGameplayAttributeData EnergyRechargeRate;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, EnergyRechargeRate);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_EnergyDrainRate)
	FGameplayAttributeData EnergyDrainRate;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, EnergyDrainRate);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, Speed);

	bool HasMaxHealth();
	bool HasMaxHunger();
	bool HasMaxEnergy();

protected:

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_HealthRegenerationRate(const FGameplayAttributeData& OldHealthRegenerationRate);

	UFUNCTION()
	virtual void OnRep_DamageRight(const FGameplayAttributeData& OldDamageRight);

	UFUNCTION()
	virtual void OnRep_DamageLeft(const FGameplayAttributeData& OldDamageLeft);

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

};
