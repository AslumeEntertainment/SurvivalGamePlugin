// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BTTask_ActivateAbilityBase.h"
#include "BTTask_ActivateAbilityFromTags.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_ActivateAbilityFromTags : public UBTTask_ActivateAbilityBase
{
	GENERATED_BODY()
	
public:

	UBTTask_ActivateAbilityFromTags();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


protected:

	UPROPERTY(EditAnywhere, Category = "Node")
	FGameplayTagContainer AbilityTags;

};
