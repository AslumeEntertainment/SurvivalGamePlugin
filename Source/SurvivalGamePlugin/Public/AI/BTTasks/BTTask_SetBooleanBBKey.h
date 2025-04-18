// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetBooleanBBKey.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_SetBooleanBBKey : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_SetBooleanBBKey();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY(EditAnywhere, Category = "Node")
	bool NewValue;

};
