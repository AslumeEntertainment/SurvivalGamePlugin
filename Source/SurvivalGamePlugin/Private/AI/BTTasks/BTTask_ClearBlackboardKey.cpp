// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_ClearBlackboardKey.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackboardKey::UBTTask_ClearBlackboardKey()
{
	NodeName = "Clear Blackboard Key";
}

EBTNodeResult::Type UBTTask_ClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->ClearValue(KeyToClear.GetSelectedKeyID());
	return EBTNodeResult::Succeeded;
}
