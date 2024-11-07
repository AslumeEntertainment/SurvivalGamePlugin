// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/HittableInterface.h"
#include "HittableFoliageActor.generated.h"

class UStaticMeshComponent;
class UPassiveDamageDealingComponent;
class ULootTableComponent;
class USG_AbilitySystemComponent;
class USG_AttributeSet;

UCLASS()
class SURVIVALGAMEPLUGIN_API AHittableFoliageActor : public AActor, public IAbilitySystemInterface, public IHittableInterface
{
	GENERATED_BODY()
	
public:	

	AHittableFoliageActor();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UPassiveDamageDealingComponent* PassiveDamageDealingComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	ULootTableComponent* LootTableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USG_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	USG_AttributeSet* AttributeSet;


public:	
	
	virtual void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles) override;

	//UFUNCTION(NetMulticast, Reliable)
	virtual void OnHittableDestroyed() override;

	virtual bool bCanBeHit() override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};
