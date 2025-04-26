// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_IsInCirclingRange.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UBTDecorator_IsInCirclingRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTDecorator_IsInCirclingRange();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) const override;

protected:

	UPROPERTY(EditAnywhere, Category = "Node")
	float OptionalRangeMultiplier = 1.f;

};
