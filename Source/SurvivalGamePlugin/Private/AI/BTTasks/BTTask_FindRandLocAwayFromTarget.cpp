// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_FindRandLocAwayFromTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_FindRandLocAwayFromTarget::UBTTask_FindRandLocAwayFromTarget()
{
	NodeName = "Find Random Location Away From Target";
}

EBTNodeResult::Type UBTTask_FindRandLocAwayFromTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandLocAwayFromTarget::ExecuteTask IsValid(World) == false"));
		return EBTNodeResult::Failed;
	}
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandLocAwayFromTarget::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	APawn* MyPawn = Controller->GetPawn();
	if (IsValid(MyPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandLocAwayFromTarget::ExecuteTask IsValid(MyPawn) == false"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandLocAwayFromTarget::ExecuteTask IsValid(Target) == false"));
		return EBTNodeResult::Failed;
	}
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if (IsValid(NavSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandLocAwayFromTarget::ExecuteTask IsValid(NavSystem) == false"));
		return EBTNodeResult::Failed;
	}

	FVector MyLocation = MyPawn->GetActorLocation();
	TArray<FVector> FoundPoints = AttemptFindTargetLocation(NavSystem, MyLocation, Target);

	if (FoundPoints.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	
	int FurthestPointId = 0;

	for (int i = 1; i < FoundPoints.Num(); i++)
	{
		if ((MyLocation - FoundPoints[i]).Length() > (MyLocation - FoundPoints[i - 1]).Length())
		{
			FurthestPointId = i;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), FoundPoints[FurthestPointId]);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}

TArray<FVector> UBTTask_FindRandLocAwayFromTarget::AttemptFindTargetLocation(UNavigationSystemV1* NavSystem, FVector MyLocation, AActor* Target)
{
	TArray<FVector> FoundPoints;

	FVector TargetLocation = Target->GetActorLocation();
	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(TargetLocation, MyLocation);
	FVector Origin = MyLocation + Direction * DistanceFromTarget;
	FNavLocation NavLocation;

	for (int i = 0; i < RetriesOnFail; i++)
	{
		for (int j = 0; j < PointsToGet; j++)
		{
			if (NavSystem->GetRandomPointInNavigableRadius(Origin, Radius, NavLocation) == false)
			{
				break;
			}

			FoundPoints.Add(NavLocation.Location);
		}

		if (FoundPoints.IsEmpty() == false)
		{
			return FoundPoints;
		}

		DistanceFromTarget -= Radius;
	}

	return FoundPoints;
}
