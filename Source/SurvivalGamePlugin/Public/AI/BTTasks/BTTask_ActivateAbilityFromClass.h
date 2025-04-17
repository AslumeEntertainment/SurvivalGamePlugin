// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BTTask_ActivateAbilityBase.h"
#include "BTTask_ActivateAbilityFromClass.generated.h"

class USG_GameplayAbility;

UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_ActivateAbilityFromClass : public UBTTask_ActivateAbilityBase
{
	GENERATED_BODY()
	
public:

	UBTTask_ActivateAbilityFromClass();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


protected:

	UPROPERTY(EditAnywhere, Category = "Node")
	TSubclassOf<USG_GameplayAbility> AbilityClass;

};
