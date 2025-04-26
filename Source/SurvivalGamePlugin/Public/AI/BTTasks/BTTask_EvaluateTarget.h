// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_EvaluateTarget.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_EvaluateTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_EvaluateTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
