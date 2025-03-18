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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void ClampAttributeOnMaxValueChanged(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

	/*---------------------------------------------Health-----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxHealth);

	/*---------------------------------------------Resistances-----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_GuardProficiency)
	FGameplayAttributeData GuardProficiency;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, GuardProficiency);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ResistanceSlashing)
	FGameplayAttributeData ResistanceSlashing;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, ResistanceSlashing);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ResistancePiercing)
	FGameplayAttributeData ResistancePiercing;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, ResistancePiercing);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ResistanceBludgeoning)
	FGameplayAttributeData ResistanceBludgeoning;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, ResistanceBludgeoning);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ResistanceFire)
	FGameplayAttributeData ResistanceFire;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, ResistanceFire);

	/*---------------------------------------------Fire Related-----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_IgnitionThreshold)
	FGameplayAttributeData IgnitionThreshold;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, IgnitionThreshold);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ExtinguishTime)
	FGameplayAttributeData ExtinguishTime;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, ExtinguishTime);

	bool HasMaxHealth() const;

protected:

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_GuardProficiency(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ResistanceSlashing(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ResistancePiercing(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ResistanceBludgeoning(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ResistanceFire(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_IgnitionThreshold(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ExtinguishTime(const FGameplayAttributeData& OldValue);

};
