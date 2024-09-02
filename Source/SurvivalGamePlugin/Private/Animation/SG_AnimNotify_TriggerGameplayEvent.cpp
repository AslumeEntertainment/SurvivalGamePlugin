// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SG_AnimNotify_TriggerGameplayEvent.h"
#include "Characters/SG_Character.h"
#include "AbilitySystemBlueprintLibrary.h"


void USG_AnimNotify_TriggerGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ASG_Character* Player = Cast<ASG_Character>(MeshComp->GetOwner());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_TriggerGameplayEvent::Received_Notify IsValid(Player) == false"));
		return;
	}

	FGameplayEventData EventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, EventTag, EventData);
	return;
}
