// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_FindRandomLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
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

	if (bNudgeTowardsClosestPlayer)
	{
		Origin += GetDirectionTowardsClosestPlayer(Character->GetWorld(), Origin) * NudgeStrength;
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

FVector UBTTask_FindRandomLocation::GetDirectionTowardsClosestPlayer(UWorld* World, FVector Origin)
{
	FVector Direction = FVector::Zero();

	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_FindRandomLocation::GetDirectionTowardsClosestPlayer IsValid(NavSystem) == false"));
		return Direction;
	}
	TArray<AActor*> Players;

	UGameplayStatics::GetAllActorsOfClass(World, APlayerController::StaticClass(), Players);

	FVector ClosestLocation = FVector::Zero();

	for (int i = 0; i < Players.Num(); i++)
	{
		APlayerController* PlayerCont = Cast<APlayerController>(Players[i]);
		if (IsValid(PlayerCont) == false)
		{
			continue;
		}
		APawn* PlayerPawn = PlayerCont->GetPawn();
		if (IsValid(PlayerPawn) == false)
		{
			continue;
		}

		if (i == 0)
		{
			ClosestLocation = PlayerPawn->GetActorLocation();
			continue;
		}

		if (UKismetMathLibrary::Vector_Distance(Origin, PlayerPawn->GetActorLocation()) < UKismetMathLibrary::Vector_Distance(Origin, ClosestLocation))
		{
			ClosestLocation = PlayerPawn->GetActorLocation();
		}
	}

	Direction = UKismetMathLibrary::GetDirectionUnitVector(Origin, ClosestLocation);

	return Direction;
}
