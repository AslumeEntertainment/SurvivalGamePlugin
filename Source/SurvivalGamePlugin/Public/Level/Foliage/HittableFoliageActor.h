// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/HittableInterface.h"
#include "HittableFoliageActor.generated.h"

class StaticMeshComponent;
class UAbilitySystemComponent;
class USG_AttributeSet;

UCLASS()
class SURVIVALGAMEPLUGIN_API AHittableFoliageActor : public AActor, public IAbilitySystemInterface, public IHittableInterface
{
	GENERATED_BODY()
	
public:	

	AHittableFoliageActor();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "DefauttAttributeValues")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "DroppedActors", meta = (ToolTip = "Value should follow this format: 000-000\nExample: 009-011"))
	TMap<TSubclassOf<AActor>, FString> ActorsToDropOnDestroyed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	USG_AttributeSet* AttributeSet;

public:	
	
	virtual void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void OnHittableDestroyed() override;

	virtual bool bCanBeHit() override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};
