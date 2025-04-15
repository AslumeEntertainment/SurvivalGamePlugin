// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_IsTargetInteractable.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Interfaces/InteractableInterface.h"
#include "AI/SG_AIController.h"


UBTService_IsTargetInteractable::UBTService_IsTargetInteractable()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Target Interactable");
}

void UBTService_IsTargetInteractable::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (IsValid(BlackBoardComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_IsTargetInRange::OnBecomeRelevant IsValid(BlackBoardComp) == false"));
		return;
	}
	ASG_AIController* Cont = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Cont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_IsTargetInRange::OnBecomeRelevant IsValid(Cont) == false"));
		return;
	}
	AActor* Target = Cont->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		return;
	}

	IInteractableInterface* InteractableTarget = Cast<IInteractableInterface>(Target);
	if (InteractableTarget == nullptr)
	{
		BlackBoardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}
	
	BlackBoardComp->SetValueAsBool(GetSelectedBlackboardKey(), InteractableTarget->bCanInteract(Cont));
}
