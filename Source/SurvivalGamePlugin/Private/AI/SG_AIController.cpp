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

void ASG_AIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (IsValid(Actor) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetDetected IsValid(Actor) == false"));
        return;
    }
    ASG_Character* OwnerCharacter = GetPawn<ASG_Character>();
    if (IsValid(OwnerCharacter) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetDetected IsValid(OwnerCharacter) == false"));
        return;
    }
    UAbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent();
    if (IsValid(ASC) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::OnTargetDetected IsValid(ASC) == false"));
        return;
    }

    GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", Stimulus.WasSuccessfullySensed());

    if (Stimulus.WasSuccessfullySensed())
    {
        CurrentTarget = Actor;
        CurrentTarget->OnDestroyed.AddDynamic(this, &ASG_AIController::OnTargetDestroyed);
        OwnerCharacter->PointCameraTowardsActor(GetWorld(), Actor);
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetSpeedAttribute());
    }
    else
    {
        CurrentTarget->OnDestroyed.RemoveDynamic(this, &ASG_AIController::OnTargetDestroyed);
        CurrentTarget = nullptr;
        OwnerCharacter->StopTarget();
        OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetSpeedAttribute()) / 2;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("%s: CurrentTarget - %s"), *GetName(), (IsValid(CurrentTarget) ? *CurrentTarget->GetName() : TEXT("NULL")));
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
