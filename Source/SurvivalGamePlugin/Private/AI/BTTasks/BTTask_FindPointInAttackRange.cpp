// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_FindPointInAttackRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_FindPointInAttackRange::UBTTask_FindPointInAttackRange()
{
	NodeName = "Find Location In Attack Range";

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindPointInAttackRange, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FindPointInAttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindPointInAttackRange::ExecuteTask IsValid(World) == false"));
		return EBTNodeResult::Failed;
	}
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindPointInAttackRange::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	APawn* MyPawn = Controller->GetPawn();
	if (IsValid(MyPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindPointInAttackRange::ExecuteTask IsValid(MyPawn) == false"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindPointInAttackRange::ExecuteTask IsValid(Target) == false"));
		return EBTNodeResult::Failed;
	}
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if (IsValid(NavSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindPointInAttackRange::ExecuteTask IsValid(NavSystem) == false"));
		return EBTNodeResult::Failed;
	}
	
	FVector TargetLocation = Target->GetActorLocation();

	TArray<FVector> FoundPoints;
	FNavLocation NavLocation;

	for (int j = 0; j < PointsToGet; j++)
	{
		if (NavSystem->GetRandomPointInNavigableRadius(TargetLocation, Controller->GetAttackRange(), NavLocation) == false)
		{
			continue;
		}
		FoundPoints.Add(NavLocation.Location);
	}

	if (FoundPoints.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	int FurthestPointId = 0;

	for (int i = 1; i < FoundPoints.Num(); i++)
	{
		if ((TargetLocation - FoundPoints[i]).Length() > (TargetLocation - FoundPoints[i - 1]).Length())
		{
			FurthestPointId = i;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), FoundPoints[FurthestPointId]);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
