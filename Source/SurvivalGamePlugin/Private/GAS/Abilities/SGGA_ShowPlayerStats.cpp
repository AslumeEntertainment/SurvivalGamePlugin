// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ShowPlayerStats.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/PlayerController.h"

#include "UI/Widgets/Game/InGameStatsWindow.h"
#include "Characters/InGameCharacter.h"


void USGGA_ShowPlayerStats::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AInGameCharacter* Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	APlayerController* PlayerCont = Cast<APlayerController>(Player->GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility IsValid(PlayerCont) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsValid(InGameStatsWindowClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility IsValid(InGameStatsWindowClass) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	InGameStatsWindow = CreateWidget<UInGameStatsWindow>(PlayerCont, InGameStatsWindowClass);
	InGameStatsWindow->AddToViewport();

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_ShowPlayerStats::CloseWindow);
	WaitInputReleaseTask->ReadyForActivation();
}

void USGGA_ShowPlayerStats::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	InGameStatsWindow->RemoveFromParent();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_ShowPlayerStats::CloseWindow(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
