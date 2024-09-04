// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTasks/SGAT_PlayMontageAndWaitForEvent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_AbilitySystemGlobals.h"
#include "GAS/Abilities/SG_GameplayAbility.h"
#include "Characters/SG_Character.h"
#include "Animation/SG_AnimInstance.h"


USGAT_PlayMontageAndWaitForEvent::USGAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

void USGAT_PlayMontageAndWaitForEvent::Activate()
{
	if (!IsValid(Ability))
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::Activate IsValid(Ability) == false"));
		return;
	}

	bool bPlayedMontage = false;

	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::Activate AbilitySystemComponent.IsValid() == false"));
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::Activate IsValid(AnimInstance) == false"));
		return;
	}

	// Bind to event callback
	EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &USGAT_PlayMontageAndWaitForEvent::OnGameplayEvent));

	if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
	{
		// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
		if (ShouldBroadcastAbilityTaskDelegates() == false)
		{
			return;
		}

		CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &USGAT_PlayMontageAndWaitForEvent::OnAbilityCancelled);

		BlendingOutDelegate.BindUObject(this, &USGAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

		MontageEndedDelegate.BindUObject(this, &USGAT_PlayMontageAndWaitForEvent::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

		ASG_Character* Character = Cast<ASG_Character>(GetAvatarActor());
		if (Character && (Character->GetLocalRole() == ROLE_Authority ||
			(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
		}

		bPlayedMontage = true;
	}

	if (!bPlayedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("USGAT_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			//ABILITY_LOG(Display, TEXT("%s: OnCancelled"), *GetName());
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void USGAT_PlayMontageAndWaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent.IsValid());
	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString USGAT_PlayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (!IsValid(Ability))
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::GetDebugString IsValid(Ability) == false"));
		return FString::Printf(TEXT("USGAT_PlayMontageAndWaitForEvent::GetDebugString IsValid(Ability) == false"));
	}
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

	if (AnimInstance != nullptr)
	{
		PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
	}

	return FString::Printf(TEXT("USGAT_PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

void USGAT_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	if (!IsValid(Ability))
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::OnDestroy IsValid(Ability) == false"));
		return;
	}
	Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);

	if (AbilityEnded && bStopWhenAbilityEnds)
	{
		StopPlayingMontage();
	}

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

USGAT_PlayMontageAndWaitForEvent* USGAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	USG_AbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	USGAT_PlayMontageAndWaitForEvent* NewTask = NewAbilityTask<USGAT_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
	NewTask->MontageToPlay = MontageToPlay;
	NewTask->EventTags = EventTags;
	NewTask->Rate = Rate;
	NewTask->StartSection = StartSection;
	NewTask->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	NewTask->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return NewTask;
}

bool USGAT_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::StopPlayingMontage ActorInfo == nullptr"));
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (IsValid(AnimInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::StopPlayingMontage IsValid(AnimInstance) == false"));
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent.IsValid() == false) 
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::StopPlayingMontage AbilitySystemComponent.IsValid() == false"));
		return false;
	}
	if (IsValid(Ability) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::StopPlayingMontage IsValid(Ability) == false"));
		return false;
	}

	if (AbilitySystemComponent->GetAnimatingAbility() == Ability && AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
	{
		// Unbind delegates so they don't get called as well
		FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
		if (MontageInstance)
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
			MontageInstance->OnMontageEnded.Unbind();
		}

		AbilitySystemComponent->CurrentMontageStop();
		return true;
	}

	return false;
}

void USGAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Ability) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut IsValid(Ability) == false"));
		return;
	}
	if (Ability->GetCurrentMontage() == MontageToPlay && Montage == MontageToPlay)
	{
		AbilitySystemComponent->ClearAnimatingAbility(Ability);

		// Reset AnimRootMotionTranslationScale
		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character && (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(1.f);
		}
	}

	if (bInterrupted && ShouldBroadcastAbilityTaskDelegates())
	{
		OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	if (!bInterrupted && ShouldBroadcastAbilityTaskDelegates())
	{
		OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void USGAT_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	if (StopPlayingMontage() && ShouldBroadcastAbilityTaskDelegates())
	{
		// Let the BP handle the interrupt as well
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void USGAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted && ShouldBroadcastAbilityTaskDelegates())
	{
		OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	EndTask();
}

void USGAT_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}
