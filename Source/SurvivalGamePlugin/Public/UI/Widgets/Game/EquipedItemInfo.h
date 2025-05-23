// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Headers/GameManagingPersistentStructs.h"
#include "Headers/PhysicalPropertyEnums.h"

#include "EquipedItemInfo.generated.h"

class UTextBlock;
class UUnitIconWidget;
class UDamageSourceData;

UCLASS()
class SURVIVALGAMEPLUGIN_API UEquipedItemInfo : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UUnitIconWidget* ItemCard;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* TXT_ItemName;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* TXT_DurabilityValue;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* TXT_WeightValue;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* TXT_DamageType;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* TXT_DamageTypeValue;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* TXT_Damage;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* TXT_DamageValue;


public:

	UFUNCTION()
	void ConfigItemCard(FResourseCardPersistentData ItemCardData);

	UFUNCTION()
	void SetItemDurability(float CurrentDurability, float MaxDurability);

	UFUNCTION()
	void SetItemWeight(EWeight ItemWeight);

	UFUNCTION()
	void SetItemDamageValues(UDamageSourceData* DamageSourceData);


};
