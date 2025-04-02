// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_GetTargetLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_GetTargetLocation::UBTTask_GetTargetLocation()
{
	NodeName = "Get Target Location";
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
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetTargetLocation::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if (IsValid(NavSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetTargetLocation::ExecuteTask IsValid(NavSystem) == false"));
		return EBTNodeResult::Failed;
	}

	FVector Origin = Target->GetActorLocation();
	FNavLocation NavLocation;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, PossibleDeviation, NavLocation) == false)
	{
		return EBTNodeResult::Failed;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NavLocation.Location);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
