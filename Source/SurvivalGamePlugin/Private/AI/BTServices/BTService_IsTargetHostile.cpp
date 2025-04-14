// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_IsTargetHostile.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Interfaces/InteractableInterface.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTService_IsTargetHostile::UBTService_IsTargetHostile()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Target Hostile");
}

void UBTService_IsTargetHostile::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);


	ASG_AIController* Cont = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Cont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_IsTargetInRange::OnBecomeRelevant IsValid(Cont) == false"));
		return;
	}
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (IsValid(BlackBoardComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_IsTargetInRange::OnBecomeRelevant IsValid(BlackBoardComp) == false"));
		return;
	}
	const AActor* Target = Cont->GetCurrentTarget();
	if (IsValid(Target) == false)
	{
		return;
	}
	AInGameCharacter* Pawn = Cont->GetPawn<AInGameCharacter>();
	if (IsValid(Pawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_IsTargetInRange::OnBecomeRelevant IsValid(Pawn) == false"));
		return;
	}
	
	BlackBoardComp->SetValueAsBool(GetSelectedBlackboardKey(), Pawn->GetTeamAttitudeTowards(*Target) == ETeamAttitude::Hostile);
}
