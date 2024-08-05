// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Items/ItemBase.h"
#include "DamageDealingItem.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API ADamageDealingItem : public AItemBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage;

public:

	virtual void OnItemEquiped(ASG_Character* NewPlayerOwner, FName SocketName) override;
	virtual void OnItemUnequiped() override;

	float GetItemDamage();

};
