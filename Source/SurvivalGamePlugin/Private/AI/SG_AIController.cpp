// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SG_AIController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"


ASG_AIController::ASG_AIController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ASG_AIController::OnTargetDetected);
    PerceptionComp->OnTargetPerceptionForgotten.AddDynamic(this, &ASG_AIController::OnTargetForgotten);
}

void ASG_AIController::OnTargetDestroyed(AActor* Target)
{
    GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", false);
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
    UAbilitySystemComponent* ASC = PossesedCharacter->GetAbilitySystemComponent();
    if (IsValid(ASC) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnPossess IsValid(ASC) == false"));
        return;
    }
    if (IsValid(BehaviorTree) == false)
    {
        UE_LOG(LogTemp, Log, TEXT("ASG_AIController::OnPossess IsValid(BehaviorTree) == false"));
        return;
    }

    PossesedCharacter->GetCharacterMovement()->MaxWalkSpeed = ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetSpeedAttribute()) / 2;

    UBlackboardComponent* BlackBoardComp = nullptr;
    UseBlackboard(BehaviorTree->BlackboardAsset, BlackBoardComp);
    Blackboard = BlackBoardComp;
    RunBehaviorTree(BehaviorTree);

    UE_LOG(LogTemp, Warning, TEXT("%s: It's AI-ing time!"), *PossesedCharacter->GetName());
}

void ASG_AIController::RegisterNewTarget(AActor* NewTarget)
{
    ASG_Character* OwnerCharacter = GetPawn<ASG_Character>();
    if (IsValid(OwnerCharacter) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::RegisterNewTarget IsValid(OwnerCharacter) == false"));
        return;
    }

    NewTarget->OnDestroyed.AddDynamic(this, &ASG_AIController::OnTargetDestroyed);

    if (Targets.IsEmpty())
    {
        Targets.Add(NewTarget);
        OwnerCharacter->PointCameraTowardsActor(GetWorld(), NewTarget);
        return;
    }

    Targets.Add(NewTarget);

    /*for (int i = 0; i < Targets.Num(); i++)
    {
        if (TargetPriorityMap[Targets[i]->StaticClass()] > TargetPriorityMap[NewTarget->StaticClass()])
        {
            Targets.Insert(NewTarget, i);
            return;
        }
        if (i == Targets.Num())
        {
            Targets.Add(NewTarget);
            return;
        }
    }*/
}

void ASG_AIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (IsValid(Actor) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetDetected IsValid(Actor) == false"));
        return;
    }

    AInGameCharacter* OwnerCharacter = GetPawn<AInGameCharacter>();
    if (IsValid(OwnerCharacter) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetDetected IsValid(OwnerCharacter) == false"));
        return;
    }

    FString TargetAttitude;
    switch (OwnerCharacter->GetTeamAttitudeTowards(*Actor))
    {
        case ETeamAttitude::Friendly: UE_LOG(LogTemp, Warning, TEXT("%s is friendly"), *Actor->GetName()); return;
        case ETeamAttitude::Neutral: TargetAttitude = "Neutral"; break;
        case ETeamAttitude::Hostile: TargetAttitude = "Hostile";
    }

    GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", Stimulus.WasSuccessfullySensed());

    FString TargetName;
    
    if (Stimulus.WasSuccessfullySensed() /*&& TargetPriorityMap.Contains(Actor->StaticClass())*/)
    {
        RegisterNewTarget(Actor);
        
        TargetName = Actor->GetName();
    }
    else if(Targets.Contains(Actor))
    {
        Targets.Remove(Actor);
        Actor->OnDestroyed.RemoveDynamic(this, &ASG_AIController::OnTargetDestroyed);

        TargetName = "NULL";
        OwnerCharacter->StopTarget(); 
    }
    
    UE_LOG(LogTemp, Warning, TEXT("%s: CurrentTarget - %s - %s"), *GetName(), *TargetName, *TargetAttitude);
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
    return Targets.IsEmpty() ? nullptr : Targets[0];
}
