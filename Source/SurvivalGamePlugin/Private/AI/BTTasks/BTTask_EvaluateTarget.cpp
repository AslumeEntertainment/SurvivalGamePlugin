// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_EvaluateTarget.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Interfaces/InteractableInterface.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_EvaluateTarget::UBTTask_EvaluateTarget()
{
	NodeName = "Evaluate Target";
}

EBTNodeResult::Type UBTTask_EvaluateTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_EvaluateTarget::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	AInGameCharacter* Pawn = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Pawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_EvaluateTarget::ExecuteTask IsValid(Pawn) == false"));
		return EBTNodeResult::Failed;
	}
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (IsValid(BlackBoardComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_EvaluateTarget::ExecuteTask IsValid(BlackBoardComp) == false"));
		return EBTNodeResult::Failed;
	}
	const AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		return EBTNodeResult::Failed;
	}

	bool bIsTargetHostile = (Pawn->GetTeamAttitudeTowards(*Target) == ETeamAttitude::Hostile) ||
		(Pawn->GetTeamAttitudeTowards(*Target) == ETeamAttitude::Friendly && Controller->bWillAttackFriendlies);

	BlackBoardComp->SetValueAsBool("IsTargetHostile", bIsTargetHostile);

	BlackBoardComp->SetValueAsBool("IsTargetInRange", Pawn->GetDistanceTo(Target) <= Pawn->GetInteractionRange());

	const IInteractableInterface* InteractableTarget = Cast<IInteractableInterface>(Target);

	BlackBoardComp->SetValueAsBool("IsTargetInteractable", (InteractableTarget != nullptr && InteractableTarget->bCanInteract(Controller)));

	return EBTNodeResult::Succeeded;
}
