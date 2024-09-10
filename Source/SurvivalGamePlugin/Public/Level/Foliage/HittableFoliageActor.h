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

	UPROPERTY(EditDefaultsOnly, Category = "DefaultAttributeValues")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "DroppedActors", meta = (ToolTip = "Value should follow this format: 000-000\nExample: 009-011"))
	TMap<TSubclassOf<AActor>, FString> ActorsToDropOnDestroyed;

	UPROPERTY(EditAnywhere, Category = "DroppedActors", meta = (ToolTip = "Should dropped actors be stacked vertically when spawned (Used for trees)"))
	bool bShouldStackVertically = false;

	UPROPERTY(EditAnywhere, Category = "DroppedActors", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"),
		meta = (ToolTip = "The percent of the foliage actor's height at which dropped actors will spawn"))
	float StartingHeightPercent = 0.5f;

	UPROPERTY(EditAnywhere, Category = "DroppedActors", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), 
		meta = (ToolTip = "How far can the dropped actors spawn from the center of the foliage actor"))
	float Spread = 0.5f;

	UPROPERTY(EditAnywhere, Category = "DroppedActors", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"),
		meta = (ToolTip = "Defines the limit to how much a dropped actor can be rotated when spawned"))
	float RotationLimit = 0.5f;

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
