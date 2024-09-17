// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/ShopWidgetBase.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include "UI/Widgets/BoosterPackWidgetBase.h"


void UShopWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Exit->OnClicked.AddDynamic(this, &UShopWidgetBase::OnExitButtonClicked);
}

void UShopWidgetBase::FillBoosters(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<uint16> Costs)
{
	SCRB_BoosterPack->ClearChildren();
	for (int i = 0; i < Names.Num(); i++)
	{
		UBoosterPackWidgetBase* BoosterWidget = CreateWidget<UBoosterPackWidgetBase>(this, BoosterPackWidgetClass);
		BoosterWidget->Config(Names[i], Images[i], Costs[i]);
		BoosterWidget->OnBodyButton.AddDynamic(this, &UShopWidgetBase::OnBoosterPackClicked);
		SCRB_BoosterPack->AddChild(BoosterWidget);
	}
}

void UShopWidgetBase::SetMoney(uint32 Money)
{
	TXT_Money->SetText(FText::FromString(FString::FromInt(Money)));
}

void UShopWidgetBase::OnExitButtonClicked()
{
	OnExitButton.Broadcast();
}

void UShopWidgetBase::OnBoosterPackClicked(UUserWidget* Widget)
{
	OnPackSelected.Broadcast(SCRB_BoosterPack->GetChildIndex(Widget));
}
