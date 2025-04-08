// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_MoveToTarget.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/SG_AIController.h"


UBTTask_MoveToTarget::UBTTask_MoveToTarget()
{
	NodeName = "Move to Target";
}

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_MoveToTarget::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}

	//FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());
	
	UAIBlueprintHelperLibrary::SimpleMoveToActor(Controller, Controller->GetCurrentTarget());

	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, TargetLocation);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
