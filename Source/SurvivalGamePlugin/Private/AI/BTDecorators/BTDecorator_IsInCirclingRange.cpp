// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorators/BTDecorator_IsInCirclingRange.h"

#include "Components/CapsuleComponent.h"
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
	AInGameCharacter* MyCharacter = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(MyCharacter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_IsInCirclingRange::CalculateRawConditionValue IsValid(MyCharacter) == false"));
		return EBTNodeResult::Failed;
	}
	AActor* Target = Controller->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_IsInCirclingRange::CalculateRawConditionValue IsValid(Target) == false"));
		return EBTNodeResult::Failed;
	}

	float TrueCircleRange =  MyCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() + Controller->GetCircleRange() * OptionalRangeMultiplier;

	return UKismetMathLibrary::Vector_Distance(MyCharacter->GetActorLocation(), Target->GetActorLocation()) <= TrueCircleRange;
}
