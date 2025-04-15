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

	ASG_AIController* Cont = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Cont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_EvaluateTarget::OnBecomeRelevant IsValid(Cont) == false"));
		return;
	}
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (IsValid(BlackBoardComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_EvaluateTarget::OnBecomeRelevant IsValid(BlackBoardComp) == false"));
		return;
	}
	const AActor* Target = Cont->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		return;
	}
	AInGameCharacter* Pawn = Cont->GetPawn<AInGameCharacter>();
	if (IsValid(Pawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_EvaluateTarget::OnBecomeRelevant IsValid(Pawn) == false"));
		return;
	}

	BlackBoardComp->SetValueAsBool("IsTargetHostile", Pawn->GetTeamAttitudeTowards(*Target) == ETeamAttitude::Hostile);

	BlackBoardComp->SetValueAsBool("IsTargetInRange", Pawn->GetDistanceTo(Target) <= Pawn->GetInteractionRange());

	const IInteractableInterface* InteractableTarget = Cast<IInteractableInterface>(Target);
	if (InteractableTarget == nullptr)
	{
		BlackBoardComp->SetValueAsBool("IsTargetInteractable", false);
		return;
	}

	BlackBoardComp->SetValueAsBool("IsTargetInteractable", InteractableTarget->bCanInteract(Cont));
}
