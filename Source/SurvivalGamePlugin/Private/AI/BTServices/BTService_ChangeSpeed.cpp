// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_ChangeSpeed.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Characters/SG_Character.h"
#include "AI/SG_AIController.h"

UBTService_ChangeSpeed::UBTService_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Change Max Speed");
}

void UBTService_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	ASG_AIController* Cont = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Cont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_ChangeSpeed::OnBecomeRelevant IsValid(Cont) == false"));
		return;
	}
	ASG_Character* Pawn = Cont->GetPawn<ASG_Character>();
	if (IsValid(Pawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_ChangeSpeed::OnBecomeRelevant IsValid(Pawn) == false"));
		return;
	}
	UAbilitySystemComponent* ASC = Pawn->GetAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_ChangeSpeed::OnBecomeRelevant IsValid(ASC) == false"));
		return;
	}

	Pawn->GetCharacterMovement()->MaxWalkSpeed = ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentSpeedAttribute()) * Multiplier;

}
