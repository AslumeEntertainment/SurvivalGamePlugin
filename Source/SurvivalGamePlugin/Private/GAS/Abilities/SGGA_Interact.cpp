// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_Interact.h"

#include "Characters/SG_Character.h"
#include "Interfaces/InteractableInterface.h"


void USGGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_Interact::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	ASG_Character* Player = Cast<ASG_Character>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_Interact::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	IInteractableInterface* FocusedInteractable = Cast<IInteractableInterface>(Player->GetFocusedInteractableObject());
	if (FocusedInteractable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_Interact::ActivateAbility FocusedInteractable == nullptr"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (FocusedInteractable->bCanInteract(Player->GetController()) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot interact!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	FocusedInteractable->InteractRequest(Player->GetController());
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
