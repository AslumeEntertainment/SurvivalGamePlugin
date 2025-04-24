// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_ActivateAbilityFromInput.h"

#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_ActivateAbilityFromInput::UBTTask_ActivateAbilityFromInput()
{
	NodeName = "Activate Ability From Input";
}

EBTNodeResult::Type UBTTask_ActivateAbilityFromInput::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromInput::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	AInGameCharacter* Character = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromInput::ExecuteTask IsValid(Character) == false"));
		return EBTNodeResult::Failed;
	}
	USG_AbilitySystemComponent* ASC = Character->GetSG_AbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromInput::ExecuteTask IsValid(ASC) == false"));
		return EBTNodeResult::Failed;
	}

	ASC->AbilityInputPressed(InputTag);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_ActivateAbilityFromInput::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromInput::AbortTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	AInGameCharacter* Character = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromInput::AbortTask IsValid(Character) == false"));
		return EBTNodeResult::Failed;
	}
	USG_AbilitySystemComponent* ASC = Character->GetSG_AbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromInput::AbortTask IsValid(ASC) == false"));
		return EBTNodeResult::Failed;
	}

	ASC->AbilityInputReleased(InputTag);

	FinishLatentAbort(OwnerComp);
	return EBTNodeResult::Aborted;
}
