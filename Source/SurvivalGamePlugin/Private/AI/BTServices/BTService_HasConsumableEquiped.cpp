// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_HasConsumableEquiped.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Interactables/Items/ConsumableItem.h"
#include "Components/EquipmentComponent.h"
#include "Characters/InGameCharacter.h"
#include "AI/SG_AIController.h"


UBTService_HasConsumableEquiped::UBTService_HasConsumableEquiped()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Has Consumable Equiped");
}

void UBTService_HasConsumableEquiped::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	ASG_AIController* Cont = Cast<ASG_AIController>(OwnerComp.GetAIOwner());
	if (IsValid(Cont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_HasConsumableEquiped::OnBecomeRelevant IsValid(Cont) == false"));
		return;
	}
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (IsValid(BlackBoardComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_HasConsumableEquiped::OnBecomeRelevant IsValid(BlackBoardComp) == false"));
		return;
	}
	AInGameCharacter* Pawn = Cont->GetPawn<AInGameCharacter>();
	if (IsValid(Pawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_HasConsumableEquiped::OnBecomeRelevant IsValid(Pawn) == false"));
		return;
	}
	UEquipmentComponent* EquipmentComp = Pawn->GetEquipmentComponent();
	if (IsValid(EquipmentComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_HasConsumableEquiped::OnBecomeRelevant IsValid(EquipmentComp) == false"));
		return;
	}

	for (AItemBase* Item : EquipmentComp->GetAllEquipedItems())
	{
		if (IsValid(Cast<AConsumableItem>(Item)))
		{
			BlackBoardComp->SetValueAsBool(GetSelectedBlackboardKey(), true);
			return;
		}
	}

	BlackBoardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
}
