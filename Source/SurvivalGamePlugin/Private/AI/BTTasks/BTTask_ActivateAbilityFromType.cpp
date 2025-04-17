// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_ActivateAbilityFromType.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "GAS/Abilities/SG_GameplayAbility.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_ActivateAbilityFromType::UBTTask_ActivateAbilityFromType()
{
	NodeName = "Activate Ability From Type";
}

EBTNodeResult::Type UBTTask_ActivateAbilityFromType::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromType::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	AInGameCharacter* Character = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromType::ExecuteTask IsValid(Character) == false"));
		return EBTNodeResult::Failed;
	}
	USG_AbilitySystemComponent* ASC = Character->GetSG_AbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromType::ExecuteTask IsValid(ASC) == false"));
		return EBTNodeResult::Failed;
	}

	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	ASC->GetActivatableGameplayAbilitySpecsFromType(AbilityType, AbilitiesToActivate);
	if (AbilitiesToActivate.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	int AbilityIndex = bPickRandom ? FMath::RandRange(0, AbilitiesToActivate.Num() - 1) : 0;

	ASC->TryActivateAbility(AbilitiesToActivate[AbilityIndex]->Handle);

	if (bWaitForCompletion == false)
	{
		return EBTNodeResult::Succeeded;
	}

	UGameplayAbility* AbilityInstance = AbilitiesToActivate[AbilityIndex]->GetPrimaryInstance();
	if (IsValid(AbilityInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromTags::ExecuteTask IsValid(AbilityInstance) == false"));
		return EBTNodeResult::Failed;
	}

	AbilityInstance->OnGameplayAbilityEnded.AddLambda(
		[&](UGameplayAbility* Ability)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);

	return EBTNodeResult::InProgress;
}
