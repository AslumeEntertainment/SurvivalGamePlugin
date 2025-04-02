// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SG_AIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Characters/InGameCharacter.h"


ASG_AIController::ASG_AIController()
{
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
   
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    PerceptionComp->ConfigureSense(*SightConfig);
    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ASG_AIController::OnTargetDetected);
    PerceptionComp->OnTargetPerceptionForgotten.AddDynamic(this, &ASG_AIController::OnTargetForgotten);
}

void ASG_AIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    AInGameCharacter* PossesedCharacter = Cast<AInGameCharacter>(InPawn);
    if (IsValid(PossesedCharacter) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnPossess IsValid(PossesedCharacter) == false"));
        return;
    }
    if (IsValid(BehaviorTree) == false)
    {
        UE_LOG(LogTemp, Log, TEXT("ASG_AIController::OnPossess IsValid(BehaviorTree) == false"));
        return;
    }

    UBlackboardComponent* BlackBoardComp = nullptr;
    UseBlackboard(BehaviorTree->BlackboardAsset, BlackBoardComp);
    Blackboard = BlackBoardComp;
    RunBehaviorTree(BehaviorTree);

    UE_LOG(LogTemp, Warning, TEXT("%s: It's AI-ing time!"), *PossesedCharacter->GetName());
}

void ASG_AIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (IsValid(Actor) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetDetected IsValid(Actor) == false"));
        return;
    }

    GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", Stimulus.WasSuccessfullySensed());

    CurrentTarget = Stimulus.WasSuccessfullySensed() ? Actor : nullptr;
    UE_LOG(LogTemp, Log, TEXT("%s: CurrentTarget - %s"), *GetName(), (IsValid(CurrentTarget) ? *CurrentTarget->GetName() : TEXT("NULL")));
}

void ASG_AIController::OnTargetForgotten(AActor* Actor)
{
    if (IsValid(Actor) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetForgotten IsValid(Actor) == false"));
        return;
    }
}

UBehaviorTree* ASG_AIController::GetBehaviorTree() const
{
    return BehaviorTree;
}

AActor* ASG_AIController::GetCurrentTarget()
{
    return CurrentTarget;
}
