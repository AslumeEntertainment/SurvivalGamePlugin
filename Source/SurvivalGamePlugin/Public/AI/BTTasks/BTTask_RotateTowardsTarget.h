// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RotateTowardsTarget.generated.h"

class ASG_Character;

UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_RotateTowardsTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_RotateTowardsTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	FTimerHandle CameraRotationTimer;

	UFUNCTION()
	void RotateTowardTarget(UWorld* World, ASG_Character* Character, AController* Controller);
};
