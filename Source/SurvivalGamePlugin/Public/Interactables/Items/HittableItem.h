// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Items/ItemBase.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/HittableInterface.h"
#include "HittableItem.generated.h"

class ULootTableComponent;
class USG_AbilitySystemComponent;
class USG_AttributeSet;

UCLASS()
class SURVIVALGAMEPLUGIN_API AHittableItem : public AItemBase, public IAbilitySystemInterface, public IHittableInterface
{
	GENERATED_BODY()
	
public:

	AHittableItem();

protected:

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	ULootTableComponent* LootTableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USG_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	USG_AttributeSet* AttributeSet;

	UFUNCTION()
	virtual void OnImpactReceived(AActor* OverlappedActor, AActor* OtherActor) override;


public:

	virtual void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles) override;

	//UFUNCTION(NetMulticast, Reliable)
	virtual void OnHittableDestroyed() override;

	virtual bool bCanBeHit() override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};
