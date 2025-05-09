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

	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void ClampAttributeOnMaxValueChanged(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

	/*---------------------------------------------Health-----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentHealth)
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, CurrentHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseHealth)
	FGameplayAttributeData BaseHealth;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, BaseHealth);

	/*---------------------------------------------Resistances-----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentGuardProficiency)
	FGameplayAttributeData CurrentGuardProficiency;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, CurrentGuardProficiency);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentMaxGuardProficiency)
	FGameplayAttributeData CurrentMaxGuardProficiency;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, CurrentMaxGuardProficiency);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxGuardProficiency)
	FGameplayAttributeData MaxGuardProficiency;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxGuardProficiency);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseGuardProficiency)
	FGameplayAttributeData BaseGuardProficiency;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, BaseGuardProficiency);



	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentSlashingDefence)
	FGameplayAttributeData CurrentSlashingDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, CurrentSlashingDefence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxSlashingDefence)
	FGameplayAttributeData MaxSlashingDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxSlashingDefence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseSlashingDefence)
	FGameplayAttributeData BaseSlashingDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, BaseSlashingDefence);



	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentPiercingDefence)
	FGameplayAttributeData CurrentPiercingDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, CurrentPiercingDefence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxPiercingDefence)
	FGameplayAttributeData MaxPiercingDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxPiercingDefence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BasePiercingDefence)
	FGameplayAttributeData BasePiercingDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, BasePiercingDefence);



	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentBludgeoningDefence)
	FGameplayAttributeData CurrentBludgeoningDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, CurrentBludgeoningDefence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxBludgeoningDefence)
	FGameplayAttributeData MaxBludgeoningDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxBludgeoningDefence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseBludgeoningDefence)
	FGameplayAttributeData BaseBludgeoningDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, BaseBludgeoningDefence);



	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrenFireDefence)
	FGameplayAttributeData CurrentFireDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, CurrentFireDefence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxFireDefence)
	FGameplayAttributeData MaxFireDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, MaxFireDefence);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseFireDefence)
	FGameplayAttributeData BaseFireDefence;
	ATTRIBUTE_ACCESSORS(USG_AttributeSet, BaseFireDefence);


	bool HasMaxHealth() const;

protected:

	UFUNCTION()
	virtual void OnRep_CurrentHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_BaseHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CurrentGuardProficiency(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_CurrentMaxGuardProficiency(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxGuardProficiency(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_BaseGuardProficiency(const FGameplayAttributeData& OldValue);


	UFUNCTION()
	virtual void OnRep_CurrentSlashingDefence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxSlashingDefence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_BaseSlashingDefence(const FGameplayAttributeData& OldValue);


	UFUNCTION()
	virtual void OnRep_CurrentPiercingDefence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxPiercingDefence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_BasePiercingDefence(const FGameplayAttributeData& OldValue);


	UFUNCTION()
	virtual void OnRep_CurrentBludgeoningDefence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxBludgeoningDefence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_BaseBludgeoningDefence(const FGameplayAttributeData& OldValue);


	UFUNCTION()
	virtual void OnRep_CurrenFireDefence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxFireDefence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_BaseFireDefence(const FGameplayAttributeData& OldValue);

};
