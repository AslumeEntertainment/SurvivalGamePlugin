// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerCustomisingWidget.h"

#include "UI/Widgets/Core/TextButtonWidgetBase.h"


void UPlayerCustomisingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TXTBTN_ReadyStatus->OnButtonClickedVoid.AddDynamic(this, &UPlayerCustomisingWidget::OnReadyButtonClicked);
	TXTBTN_LeaveTable->OnButtonClickedVoid.AddDynamic(this, &UPlayerCustomisingWidget::OnLeaveButtonClicked);
	TXTBTN_LeaveTable->SetText(FText::FromString("Leave Table"));
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

void UPlayerCustomisingWidget::CloseWidget_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerCustomisingWidget::CloseWidget_Implementation No blueprint implementation exists for function"));
	RemoveFromParent();
}
