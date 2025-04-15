// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "GameplayTagContainer.h"

#include "SG_AIController.generated.h"

class UBehaviorTree;
class UAISenseConfig_Sight;
class USG_AIPerceptionStimuliSourceComp;

USTRUCT(BlueprintType)
struct FAITarget
{
	GENERATED_BODY()

	FAITarget()
	{
		Target = nullptr;
		Priority = 0;
	}

	FAITarget(AActor* InTarget, int InPriority)
	{
		Target = InTarget;
		Priority = InPriority;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Priority;

	FORCEINLINE bool operator==(const FAITarget& Other)
	{
		return Target == Other.Target;
	}
};

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_AIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ASG_AIController();

private:

	FRotator SmoothTargetRotation;

	UPROPERTY()
	TArray<FAITarget> Targets;

	UFUNCTION()
	void OnTargetDestroyed(AActor* Target);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SmoothFocusInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (Categories = "StimuliSource"))
	TMap<FGameplayTag, int> TargetPriorityMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAISenseConfig_Sight* SightConfig = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAIPerceptionComponent* PerceptionComp = nullptr;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay) override;

	void RegisterNewTarget(AActor* NewTarget, USG_AIPerceptionStimuliSourceComp* StimuliSourceComp);

	void UnregisterTarget(AActor* NewTarget);

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);

	UFUNCTION()
	void OnTargetForgotten(AActor* Actor);

public:

	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree() const;

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTarget();

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;

};
