// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_FlipCircleDirection.h"

#include "BehaviorTree/BlackboardComponent.h"


UBTService_FlipCircleDirection::UBTService_FlipCircleDirection()
{
	bNotifyTick = true;
	NodeName = "Flip Circle Direction";

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FlipCircleDirection, BlackboardKey));
}

void UBTService_FlipCircleDirection::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	bool bNewValue = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), bNewValue);
}
