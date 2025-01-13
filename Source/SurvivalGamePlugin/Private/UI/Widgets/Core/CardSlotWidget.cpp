// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Core/CardSlotWidget.h"

#include "Components/Border.h"

#include "UI/Widgets/Core/UnitIconWidget.h"

bool UCardSlotWidget::IsEmpty() const
{
	return !SlotBorder->HasAnyChildren();
}

void UCardSlotWidget::Empty()
{
	SlotBorder->ClearChildren();
}

void UCardSlotWidget::SetCard(UUnitIconWidget* Card)
{
	Empty();
	SlotBorder->AddChild(Card);
}

UUnitIconWidget* UCardSlotWidget::GetCard()
{
	if (IsEmpty())
	{
		return nullptr;
	}
	UUnitIconWidget* Child = Cast<UUnitIconWidget>(SlotBorder->GetChildAt(0));
	if (IsValid(Child) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UCardSlotWidget::GetCard IsValid(Child) == false"));
		return nullptr;
	}
	return Child;
}
