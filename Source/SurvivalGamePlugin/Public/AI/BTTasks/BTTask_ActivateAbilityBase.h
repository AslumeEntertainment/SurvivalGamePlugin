// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ActivateAbilityBase.generated.h"

class UGameplayAbility;

UCLASS(Abstract)
class SURVIVALGAMEPLUGIN_API UBTTask_ActivateAbilityBase : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_ActivateAbilityBase();

protected:

	UPROPERTY(EditAnywhere, Category = "Node")
	bool bBlockOtherAbilitiesFromActivation = true;

	UPROPERTY(EditAnywhere, Category = "Node")
	bool bWaitForCompletion = true;

};

