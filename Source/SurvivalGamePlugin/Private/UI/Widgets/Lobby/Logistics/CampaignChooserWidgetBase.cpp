// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/Logistics/CampaignChooserWidgetBase.h"

#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"

#include "UI/Widgets/Lobby/Shop/ShopUnitWidgetBase.h"


void UCampaignChooserWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Exit->OnClicked.AddDynamic(this, &UCampaignChooserWidgetBase::OnExitButtonClicked);
}

void UCampaignChooserWidgetBase::FillCampaigns(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors, TArray<uint8> PlayerNums)
{
	UShopUnitWidgetBase* TempUnit = nullptr;

	WRPB_Campaigns->ClearChildren();
	for (int i = 0; i < Names.Num(); i++)
	{
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);

		if (IsValid(TempUnit) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UShopWidgetBase::FillSells IsValid(TempUnit) == false - Check ShopUnitWidgetClass in %s"), *GetClass()->GetName());
			return;
		}
		TempUnit->ConfigShopUnit(Names[i], Images[i], Colors[i], PlayerNums[i]);
		TempUnit->IconClicked.AddDynamic(this, &UCampaignChooserWidgetBase::OnCampaignClicked);
		WRPB_Campaigns->AddChild(TempUnit);
	}
}

void UCampaignChooserWidgetBase::Show()
{
	AddToViewport();
}

void UCampaignChooserWidgetBase::Hide()
{
	RemoveFromParent();
}

void UCampaignChooserWidgetBase::MarkForDestruction()
{
	Hide();
	Destruct();
	MarkAsGarbage();
}

void UCampaignChooserWidgetBase::OnExitButtonClicked()
{
	OnExitButton.Broadcast();
}

void UCampaignChooserWidgetBase::OnCampaignClicked(int CampaignWidgetID)
{
	OnCampaignSelected.Broadcast(CampaignWidgetID);
}