// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BTTask_ActivateAbilityBase.h"

#include "GameplayTagContainer.h"

#include "BTTask_ActivateAbilityFromInput.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_ActivateAbilityFromInput : public UBTTask_ActivateAbilityBase
{
	GENERATED_BODY()
	
public:

	UBTTask_ActivateAbilityFromInput();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


protected:

	UPROPERTY(EditAnywhere, Category = "Node")
	FGameplayTag InputTag;

};
