// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "HittableComponent.generated.h"

class UDamageSource;
class IHittableInterface;
class USG_AbilitySystemComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API UHittableComponent : public UCapsuleComponent
{
	GENERATED_BODY()
	
public:

	UHittableComponent();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "HittableDefaults")
	bool bCanDisplayDamageIndicator = true;

	UPROPERTY(EditDefaultsOnly, Category = "HittableDefaults")
	bool bCanEverReceiveGameplayEffects = true;

	UPROPERTY(EditDefaultsOnly, Category = "HittableDefaults")
	FGameplayTag OnDestroyedGameplayCueTag;

	UFUNCTION()
	void OnImpactReceived(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:

	USG_AbilitySystemComponent* GetOwnerAbilitySystemComponent();

	UFUNCTION(BlueprintCallable)
	void OnHittableDestroyed();

	UFUNCTION(BlueprintCallable)
	bool CanDisplayDamageIndicator() { return bCanDisplayDamageIndicator; }

	UFUNCTION(BlueprintCallable)
	bool CanEverReceiveGameplayEffects();

	UFUNCTION(BlueprintCallable)
	void SetCanDisplayDamageIndicator(bool NewValue);
};
