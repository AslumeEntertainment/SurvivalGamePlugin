// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_Interact.h"

#include "Characters/SG_Character.h"
#include "Interfaces/InteractableInterface.h"


void USGGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_Interact::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	ASG_Character* Avatar = Cast<ASG_Character>(ActorInfo->AvatarActor.Get());
	if (IsValid(Avatar) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_Interact::ActivateAbility IsValid(Avatar) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	IInteractableInterface* FocusedInteractable = Cast<IInteractableInterface>(Avatar->GetFocusedInteractableObject());
	if (FocusedInteractable == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (FocusedInteractable->bCanInteract(Avatar->GetController()) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s cannot interact with current focuesd interactable!"), *Avatar->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	FocusedInteractable->InteractRequest(Avatar->GetController());

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
