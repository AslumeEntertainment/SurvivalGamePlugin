// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Core/TextButtonWidgetBase.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTextButtonWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BTN->OnClicked.AddDynamic(this, &UTextButtonWidgetBase::OnButtonClicked);
}

void UTextButtonWidgetBase::SetText(FText Text)
{
	TXT->SetText(Text);
}

void UTextButtonWidgetBase::SetEnabledStatus(bool State)
{
	BTN->SetIsEnabled(State);
	TXT->SetIsEnabled(State);
}

void UTextButtonWidgetBase::OnButtonClicked()
{
	OnButtonClickedVoid.Broadcast();
	OnButtonClickedSelf.Broadcast(this);
}
