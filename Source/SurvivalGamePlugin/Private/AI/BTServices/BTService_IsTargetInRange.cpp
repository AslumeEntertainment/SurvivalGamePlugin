// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_IsTargetInRange.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/SG_Character.h"
#include "AI/SG_AIController.h"


UBTService_IsTargetInRange::UBTService_IsTargetInRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Target In Range");
}

void UBTService_IsTargetInRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

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
	ASG_Character* Pawn = Cont->GetPawn<ASG_Character>();
	if (IsValid(Pawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_IsTargetInRange::OnBecomeRelevant IsValid(Pawn) == false"));
		return;
	}
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (IsValid(BlackBoardComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_IsTargetInRange::OnBecomeRelevant IsValid(BlackBoardComp) == false"));
		return;
	}

	BlackBoardComp->SetValueAsBool(GetSelectedBlackboardKey(), Pawn->GetDistanceTo(Target) <= Pawn->GetInteractionRange());
}
