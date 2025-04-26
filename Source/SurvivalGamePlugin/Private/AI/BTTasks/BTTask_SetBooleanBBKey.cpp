// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_SetBooleanBBKey.h"

#include "BehaviorTree/BlackboardComponent.h"


UBTTask_SetBooleanBBKey::UBTTask_SetBooleanBBKey()
{
	NodeName = "Set Boolean BB Key";

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetBooleanBBKey, BlackboardKey));
}

EBTNodeResult::Type UBTTask_SetBooleanBBKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), NewValue);

	return EBTNodeResult::Succeeded;
}
