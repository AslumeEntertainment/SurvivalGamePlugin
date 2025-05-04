// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorators/BTDecorator_ShouldFlee.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Interfaces/InteractableInterface.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTDecorator_ShouldFlee::UBTDecorator_ShouldFlee()
{
	NodeName = "Should Flee";
}

bool UBTDecorator_ShouldFlee::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackBoardComp = OwnerComponent.GetBlackboardComponent();
	if (IsValid(BlackBoardComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_ShouldFlee::CalculateRawConditionValue IsValid(BlackBoardComp) == false"));
		return false;
	}
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComponent.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_ShouldFlee::CalculateRawConditionValue IsValid(Controller) == false"));
		return false;
	}
	AInGameCharacter* Pawn = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Pawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_ShouldFlee::CalculateRawConditionValue IsValid(Pawn) == false"));
		return false;
	}
	USG_AbilitySystemComponent* ASC = Pawn->GetSG_AbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_ShouldFlee::CalculateRawConditionValue IsValid(ASC) == false"));
		return false;
	}

	return BlackBoardComp->GetValueAsBool("IsTargetHostile") && BlackBoardComp->GetValueAsFloat("HealthPercent") <= MaxHealthRequired 
		&& (MustNotHaveTags.IsEmpty() || ASC->HasAllMatchingGameplayTags(MustNotHaveTags) == false);
}
