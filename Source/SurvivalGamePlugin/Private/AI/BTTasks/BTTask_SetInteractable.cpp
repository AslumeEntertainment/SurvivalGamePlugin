// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_SetInteractable.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Interfaces/InteractableInterface.h"
#include "Characters/SG_Character.h"
#include "AI/SG_AIController.h"


UBTTask_SetInteractable::UBTTask_SetInteractable()
{
	NodeName = "Set Interactable";
}

EBTNodeResult::Type UBTTask_SetInteractable::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_SetInteractable::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	ASG_Character* MyPawn = Controller->GetPawn<ASG_Character>();
	if (IsValid(MyPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_SetInteractable::ExecuteTask IsValid(MyPawn) == false"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_SetInteractable::ExecuteTask IsValid(Target) == false"));
		return EBTNodeResult::Failed;
	}
	if (Target->Implements<UInteractableInterface>() == false)
	{
		return EBTNodeResult::Failed;
	}
	
	MyPawn->Server_SetFocusedInteractable(Target);
	return EBTNodeResult::Succeeded;
}
