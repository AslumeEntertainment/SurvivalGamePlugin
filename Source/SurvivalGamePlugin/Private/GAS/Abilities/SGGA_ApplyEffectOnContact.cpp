// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ApplyEffectOnContact.h"

#include "Components/PassiveDamageDealingComponent.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"


void USGGA_ApplyEffectOnContact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AvatarActor = ActorInfo->AvatarActor.Get();
	if (IsValid(AvatarActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility IsValid(AvatarActor) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UPassiveDamageDealingComponent* PassiveDamageDealer = ActorInfo->AvatarActor.Get()->GetComponentByClass<UPassiveDamageDealingComponent>();
	if (IsValid(PassiveDamageDealer) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility IsValid(PassiveDamageDealer) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PassiveDamageDealer->OnComponentBeginOverlap.AddDynamic(this, &USGGA_ApplyEffectOnContact::OnAvatarActorBeginOverlap);
	if (bShouldApplyOnActivation == false)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	PassiveDamageDealer->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == AvatarActor && bShouldAffectUser == false)
		{
			continue;
		}
		USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor));
		if (IsValid(ASC) == false)
		{
			continue;
		}
		for (TSubclassOf<UGameplayEffect> Effect : EffectsToApply)
		{
			ASC->AddEffectFromClass(Effect, AvatarActor);
		}
	}
}

void USGGA_ApplyEffectOnContact::OnAvatarActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
	if (IsValid(ASC) == false)
	{
		return;
	}

	for (TSubclassOf<UGameplayEffect> Effect : EffectsToApply)
	{
		ASC->AddEffectFromClass(Effect, AvatarActor);
	}
}

