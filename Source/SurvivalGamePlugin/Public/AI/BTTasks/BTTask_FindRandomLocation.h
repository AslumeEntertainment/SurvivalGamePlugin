// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindRandomLocation.generated.h"

class AInGameCharacter;

UCLASS()
class SURVIVALGAMEPLUGIN_API UBTTask_FindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_FindRandomLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	float SearchRadius = 1500.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	bool bSearchInFrontOfCharater = false;

	UPROPERTY(EditAnywhere, Category = "AI")
	bool bNudgeTowardsClosestPlayer = false;

	UPROPERTY(EditAnywhere, Category = "AI")
	float NudgeStrength = 250.f;

	FVector GetDirectionTowardsClosestPlayer(UWorld* World, FVector Origin);

};
