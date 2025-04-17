// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SG_AIController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/SG_AIPerceptionStimuliSourceComp.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "Interactables/Items/ItemBase.h"
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

void ASG_AIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    GEngine->AddOnScreenDebugMessage(1, 1, FColor::Orange, FString::Printf(TEXT("Control Rotation - %s"), *GetControlRotation().ToString()));
    
    for (int i = 0; i < Targets.Num(); i++)
    {
        GEngine->AddOnScreenDebugMessage(i+2, 1, FColor::Orange, FString::Printf(TEXT("Target#%d - %s"), i, *Targets[i].Target->GetName()));
    }
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
        UE_LOG(LogTemp, Warning, TEXT("ASG_AIController::OnPossess IsValid(BehaviorTree) == false"));
        return;
    }

    PossesedCharacter->GetCharacterMovement()->MaxWalkSpeed = ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetSpeedAttribute()) / 2;

    UBlackboardComponent* BlackBoardComp = nullptr;
    UseBlackboard(BehaviorTree->BlackboardAsset, BlackBoardComp);
    Blackboard = BlackBoardComp;
    RunBehaviorTree(BehaviorTree);

    UE_LOG(LogTemp, Warning, TEXT("%s: It's AI-ing time!"), *PossesedCharacter->GetName());
}

void ASG_AIController::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
    Super::SetFocus(NewFocus, InPriority);

    ASG_Character* Char = GetPawn<ASG_Character>();
    if (IsValid(Char) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::SetFocus IsValid(ASC) == false"));
        return;
    }
    if (IsValid(NewFocus) == false)
    {
        ClearFocus(InPriority);
        Char->StopTarget();
        return;
    }
    
    Char->PointCameraTowardsActor(GetWorld(), NewFocus);
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
    IInteractableInterface* InteractableTarget = Cast<IInteractableInterface>(Actor);
    if (InteractableTarget != nullptr && InteractableTarget->bCanInteract(this) == false)
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

void ASG_AIController::RegisterNewTarget(AActor* NewTarget, USG_AIPerceptionStimuliSourceComp* StimuliSourceComp)
{
    GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", true);

    if (IsValid(StimuliSourceComp) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::RegisterNewTarget IsValid(StimuliSourceComp) == false"));
        return;
    }

    FAITarget AITarget = FAITarget(NewTarget, TargetPriorityMap[StimuliSourceComp->GetStimuliSourceTag()]);
    NewTarget->OnDestroyed.AddDynamic(this, &ASG_AIController::OnTargetDestroyed);

    if (Targets.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: There are no targets now, putting %s in front"), *GetName(), *NewTarget->GetName());

        Targets.Add(AITarget);
        SetFocus(NewTarget);

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
            UE_LOG(LogTemp, Warning, TEXT("%s: %s the new first target, locking in"), *GetName(), *NewTarget->GetName());
            SetFocus(NewTarget);
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

        NewTarget->OnDestroyed.RemoveDynamic(this, &ASG_AIController::OnTargetDestroyed);
        Targets.RemoveAt(i);

        SetFocus(Targets.IsEmpty() ? nullptr : Targets[0].Target);
        GetBlackboardComponent()->SetValueAsBool("HasVisibleTarget", !Targets.IsEmpty());

        UE_LOG(LogTemp, Warning, TEXT("%s: Current target count - %d"), *GetName(), Targets.Num());

        return;
    }
}

void ASG_AIController::StartLogic()
{
    BrainComponent->StartLogic();
}

void ASG_AIController::StopLogic()
{
    BrainComponent->StopLogic("ASG_AIController::StopLogic");
}

UBehaviorTree* ASG_AIController::GetBehaviorTree() const
{
    return BehaviorTree;
}

AActor* ASG_AIController::GetCurrentTarget()
{
    return Targets.IsEmpty() ? nullptr : Targets[0].Target;
}

void ASG_AIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
    // I wish that the intern in Epic Games that wrote the piece of
    // code that resets the pitch of the controller when the target
    // is not a Pawn dies a very painful death :)

    ASG_Character* MyCharacter = GetPawn<ASG_Character>();
    if (IsValid(MyCharacter) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ASG_AIController::UpdateControlRotation IsValid(MyCharacter) == false"));
        return;
    }

    FRotator NewControlRotation = GetControlRotation();

    const FVector FocalPoint = GetFocalPoint();
    if (FAISystem::IsValidLocation(FocalPoint))
    {
        NewControlRotation = (FocalPoint - MyCharacter->GetPawnViewLocation()).Rotation();
    }
    else if (bSetControlRotationFromPawnOrientation)
    {
        NewControlRotation = MyCharacter->GetActorRotation();
    }

    SetControlRotation(NewControlRotation);
    MyCharacter->SetRepControlRotation(NewControlRotation);

    if (bUpdatePawn == false)
    {
        return;
    }

    const FRotator CurrentPawnRotation = MyCharacter->GetActorRotation();
    if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
    {
        MyCharacter->FaceRotation(NewControlRotation, DeltaTime);
    }
}
