// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SG_AIController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Components/SG_AIPerceptionStimuliSourceComp.h"
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
    UnregisterTarget(Target);

    GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", !Targets.IsEmpty());
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

void ASG_AIController::RegisterNewTarget(AActor* NewTarget, USG_AIPerceptionStimuliSourceComp* StimuliSourceComp)
{
    GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", true);

    ASG_Character* OwnerCharacter = GetPawn<ASG_Character>();
    if (IsValid(OwnerCharacter) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::RegisterNewTarget IsValid(OwnerCharacter) == false"));
        return;
    }

    FAITarget AITarget = FAITarget(NewTarget, TargetPriorityMap[StimuliSourceComp->GetStimuliSourceTag()]);
    NewTarget->OnDestroyed.AddDynamic(this, &ASG_AIController::OnTargetDestroyed);

    if (Targets.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: There are no targets now, putting %s in front"), *GetName(), *NewTarget->GetName());
        Targets.Add(AITarget);

        OwnerCharacter->PointCameraTowardsActor(GetWorld(), NewTarget);
        
        return;
    }

    for (int i = 0; i < Targets.Num(); i++)
    {
        if (Targets[i].Priority <= AITarget.Priority)
        {
            continue;
        }

        UE_LOG(LogTemp, Warning, TEXT("%s: Inserting %s at position %d"), *GetName(), *NewTarget->GetName(), i);
        Targets.Insert(AITarget, i);

        if (i == 0)
        {
            OwnerCharacter->PointCameraTowardsActor(GetWorld(), NewTarget);
            UE_LOG(LogTemp, Warning, TEXT("%s: %s the new first target, locking in"), *GetName(), *NewTarget->GetName());
        }

        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("%s: Insering %s at the back"), *GetName(), *NewTarget->GetName());
    Targets.Add(AITarget);
}

void ASG_AIController::UnregisterTarget(AActor* NewTarget)
{
    for (int i = 0; i < Targets.Num(); i++)
    {
        if (Targets[i].Target != NewTarget)
        {
            continue;
        }

        if (i == 0)
        {
            ASG_Character* OwnerCharacter = GetPawn<ASG_Character>();
            if (IsValid(OwnerCharacter) == false)
            {
                UE_LOG(LogTemp, Error, TEXT("ASG_AIController::UnregisterTarget IsValid(OwnerCharacter) == false"));
                return;
            }
            OwnerCharacter->StopTarget();
        }

        NewTarget->OnDestroyed.RemoveDynamic(this, &ASG_AIController::OnTargetDestroyed);
        Targets.RemoveAt(i);

        UE_LOG(LogTemp, Warning, TEXT("%s: Current target count - %d"), *GetName(), Targets.Num());
        if (Targets.IsEmpty())
        {
            GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", false);
        }

        return;
    }
}

void ASG_AIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (IsValid(Actor) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetDetected IsValid(Actor) == false"));
        return;
    }
    USG_AIPerceptionStimuliSourceComp* StimuliSourceComp = Actor->GetComponentByClass<USG_AIPerceptionStimuliSourceComp>();
    if (IsValid(StimuliSourceComp) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetDetected IsValid(StimuliSourceComp) == false"));
        return;
    }

    if (TargetPriorityMap.Contains(StimuliSourceComp->GetStimuliSourceTag()) == false)
    {
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

    if (Stimulus.WasSuccessfullySensed())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: New %s target found - %s"), *GetName(), *TargetAttitude, *Actor->GetName());
        RegisterNewTarget(Actor, StimuliSourceComp);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: %s is being forgotten"), *GetName(), *Actor->GetName());
        UnregisterTarget(Actor);
    }
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
    return Targets.IsEmpty() ? nullptr : Targets[0].Target;
}
