// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetTargetLocation.generated.h"

class UNavigationSystemV1;

UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_GetTargetLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_GetTargetLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY(EditAnywhere, Category = "Node")
	float PossibleDeviation = 0.f;

	UPROPERTY(EditAnywhere, Category = "Node", meta = (ClampMin = "1"))
	int PointsToGet = 4;

	UPROPERTY(EditAnywhere, Category = "Node|OnSearchFailed", meta = (ClampMin = "1"))
	float InitialRadiusOnFailed = 50.f;

	UPROPERTY(EditAnywhere, Category = "Node|OnSearchFailed", meta = (ClampMin = "1.0"))
	float OnFailedRadiusMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Node|OnSearchFailed", meta = (ClampMin = "1"))
	int RetriesOnFail = 5;

	TArray<FVector> AttemptFindTargetLocation(UNavigationSystemV1* NavSystem, AActor* Target);

};
