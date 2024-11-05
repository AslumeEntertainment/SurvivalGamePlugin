// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_OnStartedBurning.h"

#include "AbilitySystemInterface.h"

#include "Components/PassiveDamageDealingComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interfaces/HittableInterface.h"

void USGGA_OnStartedBurning::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::ActivateAbility CommitAbility() == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	OwnerActor = ActorInfo->AvatarActor.Get();
	if (IsValid(OwnerActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::ActivateAbility ActorInfo->AvatarActor.IsValid() == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	OwnerPassiveDamageDealer = OwnerActor->GetComponentByClass<UPassiveDamageDealingComponent>();
	if (IsValid(OwnerPassiveDamageDealer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::ActivateAbility IsValid(OwnerPassiveDamageDealer) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (bShouldAttemptSpreadOnActivation)
	{
		AttemptSpreadFire();
	}
	OwnerActor->GetWorldTimerManager().SetTimer(FireSpreadTimer, this, &USGGA_OnStartedBurning::AttemptSpreadFire, Frequency, true);

	FTimerHandle AbilityDurationTimer;
	OwnerActor->GetWorldTimerManager().SetTimer(AbilityDurationTimer, this, &USGGA_OnStartedBurning::OnEffectEnded, GetAbilityLevel());
}

void USGGA_OnStartedBurning::OnEffectEnded()
{
	OwnerActor->GetWorldTimerManager().ClearTimer(FireSpreadTimer);

	UE_LOG(LogTemp, Warning, TEXT("USGGA_OnStartedBurning Effect ended"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_OnStartedBurning::AttemptSpreadFire()
{
	for (IHittableInterface* HittableActor : OwnerPassiveDamageDealer->GetOverlappedActors())
	{
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(HittableActor);
		if (ASI == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::AttemptSpreadFire ASI == nullptr"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			continue;
		}
		
		USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(ASI->GetAbilitySystemComponent());
		if (IsValid(ASC) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("USGGA_OnStartedBurning::AttemptSpreadFire IsValid(ASC) == false"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			continue;
		}

		int Amount = FMath::RandRange(1, 100);
		if (Amount <= ChanceToSpread)
		{
			ASC->AddEffectFromClass(BurningEffect, OwnerActor);
		}
	}
}
