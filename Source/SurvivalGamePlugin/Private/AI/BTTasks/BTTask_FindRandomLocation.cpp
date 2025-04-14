// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_FindRandomLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = "Find random location inside NavMesh";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandomLocation::ExecuteTask IsValid(World) == false"));
		return EBTNodeResult::Failed;
	}
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandomLocation::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	AInGameCharacter* Character = Cast<AInGameCharacter>(Controller->GetPawn());
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandomLocation::ExecuteTask IsValid(Character) == false"));
		return EBTNodeResult::Failed;
	}
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if (IsValid(NavSystem) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandomLocation::ExecuteTask IsValid(NavSystem) == false"));
		return EBTNodeResult::Failed;
	}

	FVector Origin = Character->GetActorLocation();
	if (bSearchInFrontOfCharater)
	{
		Origin += Character->GetActorForwardVector() * SearchRadius;
	}
	
	FNavLocation NavLocation;
	
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, SearchRadius, NavLocation) == false)
	{
		return EBTNodeResult::Failed;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NavLocation.Location);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
