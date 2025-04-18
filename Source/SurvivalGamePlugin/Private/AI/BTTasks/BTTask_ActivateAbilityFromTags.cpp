// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_ActivateAbilityFromTags.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "GAS/Abilities/SG_GameplayAbility.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_ActivateAbilityFromTags::UBTTask_ActivateAbilityFromTags()
{
	NodeName = "Activate Ability From Tags";
}

EBTNodeResult::Type UBTTask_ActivateAbilityFromTags::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromTags::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	AInGameCharacter* Character = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromTags::ExecuteTask IsValid(Character) == false"));
		return EBTNodeResult::Failed;
	}
	USG_AbilitySystemComponent* ASC = Character->GetSG_AbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromTags::ExecuteTask IsValid(ASC) == false"));
		return EBTNodeResult::Failed;
	}

	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTags, AbilitiesToActivate);
	if (AbilitiesToActivate.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	ASC->TryActivateAbility(AbilitiesToActivate[0]->Handle);

	UGameplayAbility* AbilityInstance = AbilitiesToActivate[0]->GetPrimaryInstance();
	if (IsValid(AbilityInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromTags::ExecuteTask IsValid(AbilityInstance) == false"));
		return EBTNodeResult::Failed;
	}

	if (bBlockOtherAbilitiesFromActivation)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);

		AbilityInstance->OnGameplayAbilityEnded.AddLambda(
			[&](UGameplayAbility* Ability)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);
			}
		);
	}
	
	if (bWaitForCompletion == false)
	{
		return EBTNodeResult::Succeeded;
	}

	AbilityInstance->OnGameplayAbilityEnded.AddLambda(
		[&](UGameplayAbility* Ability)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);
	
	return EBTNodeResult::InProgress;
}
