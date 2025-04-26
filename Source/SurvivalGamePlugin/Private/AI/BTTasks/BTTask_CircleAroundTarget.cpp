// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_CircleAroundTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_CircleAroundTarget::UBTTask_CircleAroundTarget()
{
	NodeName = "Circle Around Target";

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_CircleAroundTarget, BlackboardKey));
}

EBTNodeResult::Type UBTTask_CircleAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_CircleAroundTarget::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	APawn* MyPawn = Controller->GetPawn();
	if (IsValid(MyPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_CircleAroundTarget::ExecuteTask IsValid(MyPawn) == false"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_CircleAroundTarget::ExecuteTask IsValid(Target) == false"));
		return EBTNodeResult::Failed;
	}

	float Distance = UKismetMathLibrary::Vector_Distance(MyPawn->GetActorLocation(), Target->GetActorLocation());
	float ForwardMovementMultiplier = 0.f;

	if (Distance < Controller->GetCircleRange() - CircleRangeWidth / 2)
	{
		ForwardMovementMultiplier = -1;
	}
	else if (Distance > Controller->GetCircleRange() + CircleRangeWidth / 2)
	{
		ForwardMovementMultiplier = 1;
	}

	bool bCircleRight = OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());

	MyPawn->AddMovementInput(MyPawn->GetActorRightVector() * (bCircleRight ? 1 : -1), MovementForce);
	MyPawn->AddMovementInput(MyPawn->GetActorForwardVector() * ForwardMovementMultiplier, MovementForce);

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_CircleAroundTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FinishLatentAbort(OwnerComp);

	return EBTNodeResult::Aborted;
}

