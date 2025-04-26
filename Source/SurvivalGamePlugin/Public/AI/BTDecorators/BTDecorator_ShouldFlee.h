// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"

#include "GameplayTagContainer.h"

#include "BTDecorator_ShouldFlee.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UBTDecorator_ShouldFlee : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTDecorator_ShouldFlee();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) const override;

protected:

	UPROPERTY(EditAnywhere, Category = "Node")
	FGameplayTagContainer MustNotHaveTags;

	UPROPERTY(EditAnywhere, Category = "Node", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MaxHealthRequired = 0.25f;
};
