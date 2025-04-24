// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Perception/AIPerceptionTypes.h"
#include "GameplayTagContainer.h"

#include "SG_AIController.generated.h"

class AItemBase;
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

	UFUNCTION()
	void OnHealthUpdated(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void OnPawnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SmoothFocusInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float CircleRange = 250.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (Categories = "StimuliSource"))
	TMap<FGameplayTag, int> TargetPriorityMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAIPerceptionComponent* PerceptionComp = nullptr;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay) override;

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);

	UFUNCTION()
	void OnTargetForgotten(AActor* Actor);

	UFUNCTION()
	void HandleSightSenseUpdate(AActor* Actor, FAIStimulus const Stimulus);

	UFUNCTION()
	void HandleHearingSenseUpdate(AActor* Actor, FAIStimulus const Stimulus);

	UFUNCTION()
	void HandleTouchSenseUpdate(AActor* Actor, FAIStimulus const Stimulus);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShouldDrawDebug = true;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bWillAttackFriendlies = false;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bWillReactToFriendlySound = false;

	UFUNCTION(BlueprintCallable)
	void RegisterNewTarget(AActor* NewTarget, USG_AIPerceptionStimuliSourceComp* StimuliSourceComp);
	
	UFUNCTION(BlueprintCallable)
	void UnregisterTarget(AActor* NewTarget);

	UFUNCTION()
	void StartLogic();

	UFUNCTION()
	void StopLogic();

	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree() const;

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTarget();

	UFUNCTION(BlueprintCallable)
	float GetAttackRange();

	UFUNCTION(BlueprintCallable)
	float GetCircleRange();

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;

};
