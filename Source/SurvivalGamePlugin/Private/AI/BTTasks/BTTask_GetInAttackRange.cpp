// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_GetInAttackRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_GetInAttackRange::UBTTask_GetInAttackRange()
{
	NodeName = "Get In Attack Range";
}

EBTNodeResult::Type UBTTask_GetInAttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetInAttackRange::ExecuteTask IsValid(World) == false"));
		return EBTNodeResult::Failed;
	}
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetInAttackRange::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	APawn* MyPawn = Controller->GetPawn();
	if (IsValid(MyPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetInAttackRange::ExecuteTask IsValid(MyPawn) == false"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetInAttackRange::ExecuteTask IsValid(Target) == false"));
		return EBTNodeResult::Failed;
	}
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if (IsValid(NavSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_GetInAttackRange::ExecuteTask IsValid(NavSystem) == false"));
		return EBTNodeResult::Failed;
	}

	FVector MyLocation = MyPawn->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(TargetLocation, MyLocation);
	FVector Origin = TargetLocation + Direction * Controller->GetAttackRange();

	FNavLocation NavLocation;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, Radius, NavLocation) == false)
	{
		return EBTNodeResult::Failed;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NavLocation.Location);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
