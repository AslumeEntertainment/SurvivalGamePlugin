// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/AnnouncementWidgetBase.h"

#include "Components/TextBlock.h"

void UAnnouncementWidgetBase::SetAnnouncementTextFromString(FString AnnouncementString)
{
	if (IsValid(AnnouncementBlock) == false || AnnouncementBlock == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT(" UCoverUpWidgetBase::SetAnnouncementTextFromString IsValid(AnnouncementBlock) == false"));
		return;
	}
	AnnouncementBlock->SetText(FText::FromString(AnnouncementString));
}
