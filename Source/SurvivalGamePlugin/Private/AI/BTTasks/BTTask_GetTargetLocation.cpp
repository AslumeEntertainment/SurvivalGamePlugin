// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_GetTargetLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_GetTargetLocation::UBTTask_GetTargetLocation()
{
	NodeName = "Get Target Location";

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetTargetLocation, BlackboardKey));
}

EBTNodeResult::Type UBTTask_GetTargetLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetTargetLocation::ExecuteTask IsValid(World) == false"));
		return EBTNodeResult::Failed;
	}
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetTargetLocation::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	APawn* MyPawn = Controller->GetPawn();
	if (IsValid(MyPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetTargetLocation::ExecuteTask IsValid(MyPawn) == false"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetTargetLocation::ExecuteTask IsValid(Target) == false"));
		return EBTNodeResult::Failed;
	}
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if (IsValid(NavSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetTargetLocation::ExecuteTask IsValid(NavSystem) == false"));
		return EBTNodeResult::Failed;
	}

	TArray<FVector> FoundPoints = AttemptFindTargetLocation(NavSystem, Target);

	if (FoundPoints.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}
	
	FVector MyLocation = MyPawn->GetActorLocation();
	int ClosestPointId = 0;
	
	for (int i = 1; i < FoundPoints.Num(); i++)
	{
		if ((MyLocation - FoundPoints[i]).Length() < (MyLocation - FoundPoints[i - 1]).Length())
		{
			ClosestPointId = i;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), FoundPoints[ClosestPointId]);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}

TArray<FVector> UBTTask_GetTargetLocation::AttemptFindTargetLocation(UNavigationSystemV1* NavSystem, AActor* Target)
{
	TArray<FVector> FoundPoints;
	
	FVector Origin = Target->GetActorLocation();
	float Radius = PossibleDeviation;
	FNavLocation NavLocation;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, Radius, NavLocation))
	{
		FoundPoints.Add(NavLocation.Location);
		return FoundPoints;
	}

	Radius += InitialRadiusOnFailed;

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

		Radius *= OnFailedRadiusMultiplier;
	}
	
	return FoundPoints;
}
