// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_BasicRestoringAbility.h"
#include "Characters/SG_Character.h"
#include "Interactables/Items/RegeneratingItem.h"
#include "GAS/SG_GameplayAbilityTargetData.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Animation/SG_AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"


void USGGA_BasicRestoringAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<ASG_Character>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AnimInst = Cast<USG_AnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility IsValid(AnimInst) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (!TriggerEventData->TargetData.Data[0].IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility TriggerEventData->TargetData.Data[0].IsValid() == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	FSG_GameplayAbilityTargetData* CustomTargetData = static_cast<FSG_GameplayAbilityTargetData*>(TriggerEventData->TargetData.Data[0].Get());
	if (CustomTargetData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility CustomTargetData == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	RegeneratingItem = Cast<ARegeneratingItem>(CustomTargetData->ItemActor);
	if (!IsValid(RegeneratingItem))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_BasicRestoringAbility::ActivateAbility RegeneratingItem == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Left"))
	{
		AnimInst->bIsMirrored = true;
	}
	else if (TriggerEventData != nullptr && TriggerEventData->EventTag.ToString().Contains(".Right"))
	{
		AnimInst->bIsMirrored = false;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackAnimationMontage);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_BasicRestoringAbility::OnAnimMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_BasicRestoringAbility::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OnNotifyRecievedGameplayEventTag, nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_BasicRestoringAbility::OnEventRecieved);
	WaitGameplayEventTask->ReadyForActivation();
}

void USGGA_BasicRestoringAbility::OnAnimMontageCompleted()
{
	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_BasicRestoringAbility::OnAnimMontageCanceled()
{
	AnimInst->bIsMirrored = false;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("Ability canceled!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_BasicRestoringAbility::OnEventRecieved(FGameplayEventData Payload)
{
	FGameplayEffectContextHandle EffectContext = Player->GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(Player);

	FGameplayEffectSpecHandle SpecHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	float TimesCalled = EffectDuration / EffectPeriod + 1;
	if (EffectDuration == 0.f && EffectPeriod == 0.f)
	{
		EffectDuration = FGameplayEffectConstants::INSTANT_APPLICATION;
		EffectPeriod = FGameplayEffectConstants::NO_PERIOD;
		TimesCalled = 1.f;
	}
	
	Spec->SetDuration(EffectDuration, true);
	Spec->Period = EffectPeriod;

	float RegeneratingValue = RegeneratingItem->GetRegenerationAmount() / TimesCalled;

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, OnNotifyRecievedGameplayEventTag, RegeneratingValue);
	FActiveGameplayEffectHandle GEHandle = Player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec);

	RegeneratingItem->ServerLoseDurability(1.f);
}
