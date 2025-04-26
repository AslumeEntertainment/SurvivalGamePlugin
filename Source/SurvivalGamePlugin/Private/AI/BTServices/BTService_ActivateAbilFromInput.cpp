// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_ActivateAbilFromInput.h"

#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTService_ActivateAbilFromInput::UBTService_ActivateAbilFromInput()
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	NodeName = TEXT("Activate Ability From Input");
}

void UBTService_ActivateAbilFromInput::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASG_AIController* Controller = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_ActivateAbilFromInput::OnBecomeRelevant IsValid(Controller) == false"));
		return;
	}
	AInGameCharacter* Character = Controller->GetPawn<AInGameCharacter>();
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_ActivateAbilFromInput::OnBecomeRelevant IsValid(Character) == false"));
		return;
	}
	ASC = Character->GetSG_AbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_ActivateAbilFromInput::OnBecomeRelevant IsValid(ASC) == false"));
		return;
	}

	ASC->AbilityInputPressed(InputTag);
}

void UBTService_ActivateAbilFromInput::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASC->AbilityInputReleased(InputTag);
}
