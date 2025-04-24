// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_EvaluateTarget.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Interfaces/InteractableInterface.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTService_EvaluateTarget::UBTService_EvaluateTarget()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Evaluate Target");
}

void UBTService_EvaluateTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_EvaluateTarget::OnBecomeRelevant IsValid(Controller) == false"));
		return;
	}
	AInGameCharacter* Pawn = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Pawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_EvaluateTarget::OnBecomeRelevant IsValid(Pawn) == false"));
		return;
	}
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (IsValid(BlackBoardComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_EvaluateTarget::OnBecomeRelevant IsValid(BlackBoardComp) == false"));
		return;
	}
	const AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("No target to evaluate"));
		return;
	}
	

	bool bIsTargetHostile = (Pawn->GetTeamAttitudeTowards(*Target) == ETeamAttitude::Hostile) ||
		(Pawn->GetTeamAttitudeTowards(*Target) == ETeamAttitude::Friendly && Controller->bWillAttackFriendlies);

	BlackBoardComp->SetValueAsBool("IsTargetHostile", bIsTargetHostile);

	BlackBoardComp->SetValueAsBool("IsTargetInRange", Pawn->GetDistanceTo(Target) <= Pawn->GetInteractionRange());

	BlackBoardComp->SetValueAsBool("IsTargetInAttackRange", Pawn->GetDistanceTo(Target) <= Controller->GetAttackRange());

	const IInteractableInterface* InteractableTarget = Cast<IInteractableInterface>(Target);
	if (InteractableTarget == nullptr)
	{
		BlackBoardComp->SetValueAsBool("IsTargetInteractable", false);
		return;
	}

	BlackBoardComp->SetValueAsBool("IsTargetInteractable", InteractableTarget->bCanInteract(Controller));
}
