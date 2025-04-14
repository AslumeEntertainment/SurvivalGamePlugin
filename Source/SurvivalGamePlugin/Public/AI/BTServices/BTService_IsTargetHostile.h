// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsTargetHostile.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UBTService_IsTargetHostile : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTService_IsTargetHostile();

	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
