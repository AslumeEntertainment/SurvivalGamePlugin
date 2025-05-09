// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "GameplayEffect.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "Misc/DamageIndicator.h"


void USG_AbilitySystemComponent::Server_ShowOnDestroyedEffect_Implementation()
{
	NetMulticast_ShowOnDestroyedEffect();
}

void USG_AbilitySystemComponent::NetMulticast_ShowOnDestroyedEffect_Implementation()
{
	if (IsValid(OnDestroyedEffect) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: IsValid(OnDestroyedEffect) == false"), *GetName());
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::NetMulticast_ShowOnDestroyedEffect_Implementation IsValid(World) == false"));
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, OnDestroyedEffect, 
		GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), GetOwner()->GetActorScale3D());
}

void USG_AbilitySystemComponent::OnOwnerDestroyed()
{
	if (IsValid(GetOwner()) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("USG_AbilitySystemComponent::OwnerDestroyed IsValid(GetOwner()) == false"));
		return;
	}
	
	CancelAllAbilities();
	
	GetOwner()->HasAuthority() ? NetMulticast_ShowOnDestroyedEffect() : Server_ShowOnDestroyedEffect();
	
	TArray<FActiveGameplayEffectHandle> ActiveEffectSpecs = ActiveGameplayEffects.GetAllActiveEffectHandles();
	
	for (FActiveGameplayEffectHandle ActiveEffectSpec : ActiveEffectSpecs)
	{
		RemoveEffectFromSpec(ActiveEffectSpec);
	}
}

void USG_AbilitySystemComponent::SetStartingAttributeValues()
{
	for (TPair<FGameplayAttribute, float> StartingAttributeData : StartingAttributeSetData)
	{
		SetNumericAttributeBase(StartingAttributeData.Key, StartingAttributeData.Value);
	}
}

void USG_AbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::AbilityInputPressed InputTag.IsValid() == false"));
		return;
	}

	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTag(InputTag) == false)
		{
			continue;
		}

		if (Spec.IsActive() == false)
		{
			TryActivateAbility(Spec.Handle);
		}
		else
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void USG_AbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::AbilityInputPressed InputTag.IsValid() == false"));
		return;
	}

	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTag(InputTag) == false)
		{
			continue;
		}

		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

FActiveGameplayEffectHandle USG_AbilitySystemComponent::AddEffectFromClass(TSubclassOf<UGameplayEffect> GameplayEffectClass, const UObject* SourceObject, AActor* Instigator, AActor* EffectCauser, uint8 Level)
{
	FActiveGameplayEffectHandle GEHandle;
	
	if (IsValid(GameplayEffectClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::AddEffectFromClass IsValid(GameplayEffectClass) == false"));
		return GEHandle;
	}

	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddInstigator(Instigator, EffectCauser);
	EffectContext.AddSourceObject(SourceObject);

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffectClass, Level, EffectContext);

	if (EffectSpecHandle.IsValid())
	{
		GEHandle = ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}

	return GEHandle;
}

void USG_AbilitySystemComponent::RemoveEffectFromSpec(FActiveGameplayEffectHandle GameplayEffectSpecHandle, int8 StacksToRemove)
{
	RemoveActiveGameplayEffect(GameplayEffectSpecHandle, StacksToRemove);
}

FGameplayAbilitySpecHandle USG_AbilitySystemComponent::AddAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, int Level, UObject* SourceObject, bool bAllowDuplicates)
{
	if (bAllowDuplicates == false)
	{
		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (Spec.Ability.GetClass() == AbilityClass)
			{
				return FGameplayAbilitySpecHandle();
			}
		}
	}
	FGameplayAbilitySpec Spec = FGameplayAbilitySpec(AbilityClass, Level, INDEX_NONE, SourceObject);

	if (const USG_GameplayAbility* SG_Ability = Cast<USG_GameplayAbility>(Spec.Ability))
	{
		Spec.DynamicAbilityTags.AddTag(SG_Ability->InputTag);
		return GiveAbility(Spec);
	}

	return FGameplayAbilitySpecHandle();
}

void USG_AbilitySystemComponent::AddAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* SourceObject, bool bAllowDuplicates)
{
	for (const TPair<TSubclassOf<USG_GameplayAbility>, int> Ability : AbilityMap)
	{
		AddAbility(Ability.Key, Ability.Value, SourceObject, bAllowDuplicates);
	}
}

FGameplayAbilitySpecHandle USG_AbilitySystemComponent::AddPassiveAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, int Level, UObject* SourceObject, bool bAllowDuplicates)
{
	if (bAllowDuplicates == false)
	{
		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (Spec.Ability.GetClass() == AbilityClass)
			{
				return FGameplayAbilitySpecHandle();
			}
		}
	}
	
	FGameplayAbilitySpec Spec = FGameplayAbilitySpec(AbilityClass, Level, INDEX_NONE, SourceObject);
	return GiveAbilityAndActivateOnce(Spec);
}

void USG_AbilitySystemComponent::AddPassiveAbilitiesFromMap(TMap<TSubclassOf<USG_GameplayAbility>, int> AbilityMap, UObject* SourceObject, bool bAllowDuplicates)
{
	for (TPair<TSubclassOf<USG_GameplayAbility>, int> Ability : AbilityMap)
	{
		AddPassiveAbility(Ability.Key, Ability.Value, SourceObject, bAllowDuplicates);
	}
}

void USG_AbilitySystemComponent::RemoveAbilityFromSpec(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle)
{
	ClearAbility(RemovedAbilitySpecHandle);
}

void USG_AbilitySystemComponent::GetActivatableGameplayAbilitySpecFromClass(TSubclassOf<UGameplayAbility> AbilityClass, FGameplayAbilitySpec*& MatchingGameplayAbility) const
{
	if (IsValid(AbilityClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::GetActivatableGameplayAbilitySpecFromClass IsValid(AbilityClass) == false"));
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (IsValid(Spec.Ability) == false || Spec.Ability->GetClass() != AbilityClass)
		{
			continue;
		}
		
		if (Spec.Ability->DoesAbilitySatisfyTagRequirements(*this) == false)
		{
			continue;
		}

		MatchingGameplayAbility = const_cast<FGameplayAbilitySpec*>(&Spec);
		return;
	}
}

void USG_AbilitySystemComponent::GetActivatableGameplayAbilitySpecsFromType(FGameplayTag AbilityTypeTag, TArray<struct FGameplayAbilitySpec*>& MatchingGameplayAbilities) const
{
	if (AbilityTypeTag.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AbilitySystemComponent::GetActivatableGameplayAbilitySpecsFromType AbilityTypeTag.IsValid() == false"));
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		USG_GameplayAbility* Ability = Cast<USG_GameplayAbility>(Spec.Ability);
		if (IsValid(Ability) == false || Ability->AbilityTypeTag != AbilityTypeTag)
		{
			continue;
		}

		if (Ability->DoesAbilitySatisfyTagRequirements(*this) == false)
		{
			continue;
		}

		MatchingGameplayAbilities.Add(const_cast<FGameplayAbilitySpec*>(&Spec));
	}
}


