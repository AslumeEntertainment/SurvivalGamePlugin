// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerCustomisingWidget.h"

#include "UI/Widgets/Core/TextButtonWidgetBase.h"



void UPlayerCustomisingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TXTBTN_ReadyStatus->OnButtonClickedVoid.AddDynamic(this, &UPlayerCustomisingWidget::OnReadyButtonClicked);
	TXTBTN_LeaveTable->OnButtonClickedVoid.AddDynamic(this, &UPlayerCustomisingWidget::OnLeaveButtonClicked);

}

void UPlayerCustomisingWidget::OnReadyButtonClicked()
{
	OnReadyButton.Broadcast();
}

void UPlayerCustomisingWidget::OnLeaveButtonClicked()
{
	OnLeaveButton.Broadcast();
}

void UPlayerCustomisingWidget::Config(EPlayerReadinessUIStatus PlayerReadiness)
{
	TXTBTN_ReadyStatus->SetText(FText::FromString(PlayerReadiness == EPlayerReadinessUIStatus::Ready ? "Cancel" : (PlayerReadiness == EPlayerReadinessUIStatus::NotReady ? "Ready" : "Updating Status")));
}
