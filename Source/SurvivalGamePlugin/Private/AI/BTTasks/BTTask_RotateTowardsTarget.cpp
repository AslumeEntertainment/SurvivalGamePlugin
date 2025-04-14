// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_RotateTowardsTarget.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/SG_AIController.h"


UBTTask_RotateTowardsTarget::UBTTask_RotateTowardsTarget()
{
	NodeName = "Rotate Towards Target";
}

EBTNodeResult::Type UBTTask_RotateTowardsTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_RotateTowardsTarget::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}

void UBTTask_RotateTowardsTarget::RotateTowardTarget(UWorld* World, ASG_Character* Character, AController* Controller)
{
}
