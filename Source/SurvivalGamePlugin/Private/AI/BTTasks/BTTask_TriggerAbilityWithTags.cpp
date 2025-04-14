// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_TriggerAbilityWithTags.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_TriggerAbilityWithTags::UBTTask_TriggerAbilityWithTags()
{
	NodeName = "Trigger Ability With Tags";
}

EBTNodeResult::Type UBTTask_TriggerAbilityWithTags::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_TriggerAbilityWithTags::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	AInGameCharacter* Character = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_TriggerAbilityWithTags::ExecuteTask IsValid(Character) == false"));
		return EBTNodeResult::Failed;
	}
	USG_AbilitySystemComponent* ASC = Character->GetSG_AbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_TriggerAbilityWithTags::ExecuteTask IsValid(ASC) == false"));
		return EBTNodeResult::Failed;
	}

	ASC->TryActivateAbilitiesByTag(AbilityTags);

	return EBTNodeResult::Succeeded;
}
