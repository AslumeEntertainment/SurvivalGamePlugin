// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Game/EquipedItemInfo.h"

#include "Components/TextBlock.h"

#include "Objects/GameplayEffectDataObjects/DamageSourceData.h"
#include "UI/Widgets/Core/UnitIconWidget.h"

void UEquipedItemInfo::ConfigItemCard(FResourseCardPersistentData ItemCardData)
{
	TXT_ItemName->SetText(ItemCardData.UnitName);
	ItemCard->SetItemName(ItemCardData.UnitName);
	ItemCard->SetItemImage(ItemCardData.UnitImage);
	ItemCard->SetItemColor(ItemCardData.UnitColor);
}

void UEquipedItemInfo::SetItemDurability(float CurrentDurability, float MaxDurability)
{
	TXT_DurabilityValue->SetText(FText::FromString(FString::Printf(TEXT("%.f/%.f"), CurrentDurability, MaxDurability)));
}

void UEquipedItemInfo::SetItemWeight(EWeight ItemWeight)
{
	switch (ItemWeight)
	{
	case EWeight::None:
		TXT_WeightValue->SetText(FText::FromString("None")); break;
	case EWeight::VeryLight:
		TXT_WeightValue->SetText(FText::FromString("Very Light")); break;
	case EWeight::Light:
		TXT_WeightValue->SetText(FText::FromString("Light")); break;
	case EWeight::Normal:
		TXT_WeightValue->SetText(FText::FromString("Normal")); break;
	case EWeight::Heavy:
		TXT_WeightValue->SetText(FText::FromString("Heavy")); break;
	case EWeight::VeryHeavy:
		TXT_WeightValue->SetText(FText::FromString("VeryHeavy"));
	}
}

void UEquipedItemInfo::SetItemDamageValues(UDamageSourceData* DamageSourceData)
{
	if (IsValid(DamageSourceData) == false)
	{
		TXT_DamageType->SetRenderOpacity(0.f);
		TXT_DamageTypeValue->SetRenderOpacity(0.f);
		TXT_Damage->SetRenderOpacity(0.f);
		TXT_DamageValue->SetRenderOpacity(0.f);
	}
	else
	{
		TXT_DamageType->SetRenderOpacity(1.f);
		TXT_DamageTypeValue->SetRenderOpacity(1.f);
		TXT_Damage->SetRenderOpacity(1.f);
		TXT_DamageValue->SetRenderOpacity(1.f);

		TXT_DamageValue->SetText(FText::FromString(FString::Printf(TEXT("%.f"), DamageSourceData->BaseDamage)));

		switch(DamageSourceData->DamageType)
		{
			case ESG_DamageType::Slashing: 
				TXT_DamageTypeValue->SetText(FText::FromString("Slashing")); break;
			case ESG_DamageType::Piercing:
				TXT_DamageTypeValue->SetText(FText::FromString("Piercing")); break;
			case ESG_DamageType::Bludgeoning:
				TXT_DamageTypeValue->SetText(FText::FromString("Bludgeoning")); break;
			case ESG_DamageType::Fire:
				TXT_DamageTypeValue->SetText(FText::FromString("Fire")); break;
			case ESG_DamageType::None:
				TXT_DamageTypeValue->SetText(FText::FromString("None"));
		}
	}
}
