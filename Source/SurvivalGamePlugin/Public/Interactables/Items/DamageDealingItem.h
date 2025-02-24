// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Items/ItemBase.h"
#include "DamageDealingItem.generated.h"

class UDamageDealingComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API ADamageDealingItem : public AItemBase
{
	GENERATED_BODY()
	
public:

	ADamageDealingItem();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UDamageDealingComponent* DamageDealingComponent;

	UFUNCTION()
	void CalculateDurabilityLost(UObject* HitObject);

public:

	virtual void OnItemEquiped(AInGameCharacter* PlayerOwner, FName SocketName) override;

	virtual void OnItemUnequiped() override;

	UDamageDealingComponent* GetDamageDealingComponent();

};
