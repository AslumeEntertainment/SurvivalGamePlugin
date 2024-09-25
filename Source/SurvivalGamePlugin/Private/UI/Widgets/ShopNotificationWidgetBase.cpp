// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/ShopNotificationWidgetBase.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UShopNotificationWidgetBase::NativeOnInitialized()
{

	Super::NativeOnInitialized();
	BTN_Ok->OnClicked.AddDynamic(this, &UShopNotificationWidgetBase::OnOKButtonClicked);
	BTN_Cancel->OnClicked.AddDynamic(this, &UShopNotificationWidgetBase::OnCancelButtonClicked);
}

void UShopNotificationWidgetBase::Config(bool CanClickOk, FText Action, FText Object, uint16 Cost)
{
	TXT_Action->SetText(Action);
	TXT_Object->SetText(Object);
	TXT_Cost->SetText(FText::FromString(FString::FromInt(Cost)));
	BTN_Ok->SetIsEnabled(CanClickOk);
}

void UShopNotificationWidgetBase::OnOKButtonClicked()
{
	OnOkButton.Broadcast();
}

void UShopNotificationWidgetBase::OnCancelButtonClicked()
{
	OnCancelButton.Broadcast();
}


