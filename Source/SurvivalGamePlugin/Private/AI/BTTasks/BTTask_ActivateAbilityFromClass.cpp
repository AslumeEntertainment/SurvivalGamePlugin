// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_ActivateAbilityFromClass.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "GAS/Abilities/SG_GameplayAbility.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTTask_ActivateAbilityFromClass::UBTTask_ActivateAbilityFromClass()
{
	NodeName = "Activate Ability From Class";
}

EBTNodeResult::Type UBTTask_ActivateAbilityFromClass::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromClass::ExecuteTask IsValid(Controller) == false"));
		return EBTNodeResult::Failed;
	}
	AInGameCharacter* Character = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromClass::ExecuteTask IsValid(Character) == false"));
		return EBTNodeResult::Failed;
	}
	USG_AbilitySystemComponent* ASC = Character->GetSG_AbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromClass::ExecuteTask IsValid(ASC) == false"));
		return EBTNodeResult::Failed;
	}

	FGameplayAbilitySpec* AbilityToActivate = nullptr;
	ASC->GetActivatableGameplayAbilitySpecFromClass(AbilityClass, AbilityToActivate);
	if (AbilityToActivate == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromClass::ExecuteTask AbilityToActivate == nullptr"));
		return EBTNodeResult::Failed;
	}

	ASC->TryActivateAbility(AbilityToActivate->Handle);

	if (bWaitForCompletion == false)
	{
		return EBTNodeResult::Succeeded;
	}

	UGameplayAbility* AbilityInstance = AbilityToActivate->GetPrimaryInstance();
	if (IsValid(AbilityInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_ActivateAbilityFromClass::ExecuteTask IsValid(AbilityInstance) == false"));
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
