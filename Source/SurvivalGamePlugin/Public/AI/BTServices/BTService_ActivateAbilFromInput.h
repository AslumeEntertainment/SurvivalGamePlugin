// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"

#include "GameplayTagContainer.h"

#include "BTService_ActivateAbilFromInput.generated.h"

class USG_AbilitySystemComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API UBTService_ActivateAbilFromInput : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTService_ActivateAbilFromInput();

	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	USG_AbilitySystemComponent* ASC = nullptr;

	UPROPERTY(EditAnywhere, Category = "Node")
	FGameplayTag InputTag;
};
