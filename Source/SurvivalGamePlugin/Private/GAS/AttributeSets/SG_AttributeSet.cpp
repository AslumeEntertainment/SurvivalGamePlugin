// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Characters/SG_Character.h"
#include "Components/DamageDealingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/HittableInterface.h"
#include "Net/UnrealNetwork.h"


USG_AttributeSet::USG_AttributeSet()
{
}

void USG_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USG_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void USG_AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHealth());
	}
}

void USG_AttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		IHittableInterface* AttributeOwner = Cast<IHittableInterface>(GetOwningActor());
		if (AttributeOwner == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange AttributeOwner == nullptr"));
			return;
		}
		UAbilitySystemComponent* OwnerAbilitySystemComponent = GetOwningAbilitySystemComponent();
		if (!IsValid(OwnerAbilitySystemComponent))
		{
			UE_LOG(LogTemp, Error, TEXT("USG_AttributeSet::PostAttributeBaseChange IsValid(OwnerAbilitySystemComponent) == false"));
			return;
		}

		if (HasMaxHealth())
		{
			OwnerAbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
		}
		else
		{
			OwnerAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasFullHealth")));
		}

		if (GetHealth() == 0.f)
		{
			OwnerAbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Bool.HasNoHealth")));
			AttributeOwner->OnHittableDestroyed();
		}

		ASG_Character* AttributeOwnerCharacter = Cast<ASG_Character>(GetOwningActor());
		if (IsValid(AttributeOwnerCharacter))
		{
			AttributeOwnerCharacter->OnUpdatePlayerHealth(GetHealth(), GetMaxHealth());
		}
	}
}

void USG_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

bool USG_AttributeSet::HasMaxHealth() const
{
	return GetHealth() == GetMaxHealth();
}

void USG_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, Health, OldHealth);
}

void USG_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USG_AttributeSet, MaxHealth, OldMaxHealth);
}
