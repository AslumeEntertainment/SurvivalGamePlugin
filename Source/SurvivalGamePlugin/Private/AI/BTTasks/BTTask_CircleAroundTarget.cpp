// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_CircleAroundTarget.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_CircleAroundTarget::UBTTask_CircleAroundTarget()
{
	NodeName = "Circle Around Target";
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

	MyPawn->AddMovementInput(MyPawn->GetActorRightVector() * (bFlipDirection ? -1 : 1));

	return EBTNodeResult::Succeeded;
}

