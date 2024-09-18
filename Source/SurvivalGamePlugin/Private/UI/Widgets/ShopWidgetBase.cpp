// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/ShopWidgetBase.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include "UI/Widgets/ShopUnitWidgetBase.h"


void UShopWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Exit->OnClicked.AddDynamic(this, &UShopWidgetBase::OnExitButtonClicked);
}

void UShopWidgetBase::FillBoosters(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<uint16> Costs)
{
	UShopUnitWidgetBase* TempUnit = nullptr;

	SCRB_BoosterPack->ClearChildren();
	for (int i = 0; i < Names.Num(); i++)
	{
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);
		if (IsValid(TempUnit) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UShopWidgetBase::FillBoosters IsValid(TempUnit) == false - Check ShopUnitWidgetClass in %s"), *GetClass()->GetName());
			return;
		}
		TempUnit->Config(Names[i], Images[i], Costs[i]);
		TempUnit->OnBodyButton.AddDynamic(this, &UShopWidgetBase::OnBoosterPackClicked);
		SCRB_BoosterPack->AddChild(TempUnit);
	}
}

void UShopWidgetBase::FillSells(TArray<FText> HeroNames, TArray<UTexture2D*> HeroImages, TArray<uint16> HeroCosts, TArray<FText> ResourceNames, TArray<UTexture2D*> ResourceImages, TArray<uint16> ResourceCosts)
{
	UShopUnitWidgetBase* TempUnit = nullptr;

	SCRB_Heroes->ClearChildren();
	for (int i = 0; i < HeroNames.Num(); i++)
	{
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);
		if (IsValid(TempUnit) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UShopWidgetBase::FillSells IsValid(TempUnit) == false - Check ShopUnitWidgetClass in %s"), *GetClass()->GetName());
			return;
		}
		TempUnit->Config(HeroNames[i], HeroImages[i], HeroCosts[i]);
		TempUnit->OnBodyButton.AddDynamic(this, &UShopWidgetBase::OnHeroCardClicked);
		SCRB_Heroes->AddChild(TempUnit);
	}

	SCRB_Resources->ClearChildren();
	for (int i = 0; i < ResourceNames.Num(); i++)
	{
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);
		if (IsValid(TempUnit) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UShopWidgetBase::FillSells IsValid(TempUnit) == false - Check ShopUnitWidgetClass in %s"), *GetClass()->GetName());
			return;
		}
		TempUnit->Config(ResourceNames[i], ResourceImages[i], ResourceCosts[i]);
		TempUnit->OnBodyButton.AddDynamic(this, &UShopWidgetBase::OnResourceCardClicked);
		SCRB_Resources->AddChild(TempUnit);
	}
}

void UShopWidgetBase::SetMoney(uint32 Money)
{
	TXT_Money->SetText(FText::FromString(FString::FromInt(Money)));
}

void UShopWidgetBase::RemoveResourceCard(uint16 Index)
{
	SCRB_Resources->RemoveChildAt(Index);
}

void UShopWidgetBase::RemoveHeroCard(uint16 Index)
{
	SCRB_Heroes->RemoveChildAt(Index);
}

void UShopWidgetBase::OnExitButtonClicked()
{
	OnExitButton.Broadcast();
}

void UShopWidgetBase::OnBoosterPackClicked(UUserWidget* Widget)
{
	OnPackSelected.Broadcast(SCRB_BoosterPack->GetChildIndex(Widget));
}

void UShopWidgetBase::OnHeroCardClicked(UUserWidget* Widget)
{
	OnHeroCardSelected.Broadcast(SCRB_Heroes->GetChildIndex(Widget));
}

void UShopWidgetBase::OnResourceCardClicked(UUserWidget* Widget)
{
	OnResourceCardSelected.Broadcast(SCRB_Resources->GetChildIndex(Widget));
}