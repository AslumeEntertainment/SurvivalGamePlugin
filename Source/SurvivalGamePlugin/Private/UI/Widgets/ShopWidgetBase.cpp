// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/ShopWidgetBase.h"

#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"

#include "UI/Widgets/ShopUnitWidgetBase.h"


void UShopWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Exit->OnClicked.AddDynamic(this, &UShopWidgetBase::OnExitButtonClicked);
}

void UShopWidgetBase::FillBoosters(TArray<FText> Names, TArray<UTexture2D*> Images, TArray<FLinearColor> Colors, TArray<uint16> Costs)
{
	UShopUnitWidgetBase* TempUnit = nullptr;

	WRPB_BoosterPack->ClearChildren();
	for (int i = 0; i < Names.Num(); i++)
	{
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);
		if (IsValid(TempUnit) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UShopWidgetBase::FillBoosters IsValid(TempUnit) == false - Check ShopUnitWidgetClass in %s"), *GetClass()->GetName());
			return;
		}
		TempUnit->ConfigShopUnit(Names[i], Images[i], Colors[i], Costs[i], BoosterPackWidgetScale);
		TempUnit->IconClicked.AddDynamic(this, &UShopWidgetBase::OnBoosterPackClicked);
		WRPB_BoosterPack->AddChild(TempUnit);
	}
}

void UShopWidgetBase::FillHeroes(TArray<FText> HeroNames, TArray<UTexture2D*> HeroImages, TArray<FLinearColor> HeroColors, TArray<uint16> HeroCosts)
{
	UShopUnitWidgetBase* TempUnit = nullptr;

	WRPB_Heroes->ClearChildren();
	for (int i = 0; i < HeroNames.Num(); i++)
	{
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);
		if (IsValid(TempUnit) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UShopWidgetBase::FillHeroes IsValid(TempUnit) == false - Check ShopUnitWidgetClass in %s"), *GetClass()->GetName());
			return;
		}
		TempUnit->ConfigShopUnit(HeroNames[i], HeroImages[i], HeroColors[i], HeroCosts[i], HeroCardWidgetScale);
		TempUnit->IconClicked.AddDynamic(this, &UShopWidgetBase::OnHeroCardClicked);
		WRPB_Heroes->AddChild(TempUnit);
	}
}

void UShopWidgetBase::FillResources(TArray<FText> ResourceNames, TArray<UTexture2D*> ResourceImages, TArray<FLinearColor> ResourceColors, TArray<uint16> ResourceCosts)
{
	UShopUnitWidgetBase* TempUnit = nullptr;

	WRPB_Resources->ClearChildren();
	for (int i = 0; i < ResourceNames.Num(); i++)
	{
		TempUnit = CreateWidget<UShopUnitWidgetBase>(this, ShopUnitWidgetClass);
		if (IsValid(TempUnit) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UShopWidgetBase::FillResources IsValid(TempUnit) == false - Check ShopUnitWidgetClass in %s"), *GetClass()->GetName());
			return;
		}
		TempUnit->ConfigShopUnit(ResourceNames[i], ResourceImages[i], ResourceColors[i], ResourceCosts[i], ResourceCardWidgetScale);
		TempUnit->IconClicked.AddDynamic(this, &UShopWidgetBase::OnResourceCardClicked);
		WRPB_Resources->AddChild(TempUnit);
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

void UShopWidgetBase::OnBoosterPackClicked(int WidgetIndex)
{
	OnPackSelected.Broadcast(WidgetIndex);
}

void UShopWidgetBase::OnHeroCardClicked(int WidgetIndex)
{
	OnHeroCardSelected.Broadcast(WidgetIndex);
}

void UShopWidgetBase::OnResourceCardClicked(int WidgetIndex)
{
	OnResourceCardSelected.Broadcast(WidgetIndex);
}