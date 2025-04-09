// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Perception/AIPerceptionTypes.h"

#include "SG_AIController.generated.h"

class UBehaviorTree;
class UAISenseConfig_Sight;

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_AIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ASG_AIController();

private:

	UPROPERTY()
	TArray<AActor*> Targets;

	UFUNCTION()
	void OnTargetDestroyed(AActor* Target);


protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI")
	TMap<TSubclassOf<AActor>, int> TargetPriorityMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAISenseConfig_Sight* SightConfig = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAIPerceptionComponent* PerceptionComp = nullptr;

	virtual void OnPossess(APawn* InPawn) override;

	void RegisterNewTarget(AActor* NewTarget);

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);

	UFUNCTION()
	void OnTargetForgotten(AActor* Actor);

public:

	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree() const;

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTarget();

};
