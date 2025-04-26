// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorators/BTDecorator_IsInCirclingRange.h"

#include "Kismet/KismetMathLibrary.h"

#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTDecorator_IsInCirclingRange::UBTDecorator_IsInCirclingRange()
{
	NodeName = "Is In Circling Range";
}

bool UBTDecorator_IsInCirclingRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) const
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComponent.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_IsInCirclingRange::CalculateRawConditionValue IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	APawn* MyPawn = Controller->GetPawn();
	if (IsValid(MyPawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_IsInCirclingRange::CalculateRawConditionValue IsValid(MyPawn) == false"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_IsInCirclingRange::CalculateRawConditionValue IsValid(Target) == false"));
		return EBTNodeResult::Failed;
	}

	return UKismetMathLibrary::Vector_Distance(MyPawn->GetActorLocation(), Target->GetActorLocation()) <= Controller->GetCircleRange() * OptionalRangeMultiplier;
}
