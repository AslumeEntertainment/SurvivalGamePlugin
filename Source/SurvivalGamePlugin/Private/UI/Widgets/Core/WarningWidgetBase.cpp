// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Core/WarningWidgetBase.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void UWarningWidgetBase::ConfirmButtonClicked()
{
	OnConfirmButtonClicked.Broadcast();
}

void UWarningWidgetBase::CancelButtonClicked()
{
	OnCancelButtonClicked.Broadcast();
}

void UWarningWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Confirm->OnClicked.AddDynamic(this, &UWarningWidgetBase::ConfirmButtonClicked);
	BTN_Cancel->OnClicked.AddDynamic(this, &UWarningWidgetBase::CancelButtonClicked);
}

void UWarningWidgetBase::ConfigWarning(FText Message, bool CanClickConfirm, FText ConfirmText, FText CancelText)
{
	TXT_Message->SetText(Message);
	TXT_ConfirmText->SetText(ConfirmText);
	TXT_CancelText->SetText(CancelText);
	BTN_Confirm->SetIsEnabled(CanClickConfirm);
}



