// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "SG_HittableActor.generated.h"

class UStaticMeshComponent;
class UHittableComponent;
class ULootTableComponent;
class USG_AbilitySystemComponent;
class UAbilitySystemComponent;
class USG_AttributeSet;

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_HittableActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	
	ASG_HittableActor();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> InitializingEffect;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UHittableComponent* HittableComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	ULootTableComponent* LootTableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USG_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	USG_AttributeSet* AttributeSet;

public:	

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UStaticMeshComponent* GetStaticMeshComponent();

	UHittableComponent* GetHittableComponent();

	ULootTableComponent* GetLootTableComponent();

	UFUNCTION(BlueprintCallable)
	virtual void DestroyHittableActor(bool bSafeDestruct = false);

};
