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

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void ClampAttributeOnMaxValueChanged(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxHealth);

	/* Fire related attributes */

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_IgnitionThreshold)
	FGameplayAttributeData IgnitionThreshold;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, IgnitionThreshold);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_FireDamageResistance)
	FGameplayAttributeData FireDamageResistance;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, FireDamageResistance);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ExtinguishTime)
	FGameplayAttributeData ExtinguishTime;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, ExtinguishTime);

	bool HasMaxHealth() const;

protected:

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_IgnitionThreshold(const FGameplayAttributeData& OldIgnitionThreshold);

	UFUNCTION()
	virtual void OnRep_FireDamageResistance(const FGameplayAttributeData& OldFireDamageResistance);

	UFUNCTION()
	virtual void OnRep_ExtinguishTime(const FGameplayAttributeData& OldExtinguishTime);

};
