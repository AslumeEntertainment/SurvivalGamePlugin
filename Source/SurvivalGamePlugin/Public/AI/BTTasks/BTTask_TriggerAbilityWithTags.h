// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "GameplayTagContainer.h"

#include "BTTask_TriggerAbilityWithTags.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_TriggerAbilityWithTags : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_TriggerAbilityWithTags();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	FGameplayTagContainer AbilityTags;

};
