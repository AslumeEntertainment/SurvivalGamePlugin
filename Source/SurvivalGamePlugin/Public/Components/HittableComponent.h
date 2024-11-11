// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Headers/DamageGameplayEffectStruct.h"
#include "HittableComponent.generated.h"

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

	UPROPERTY()
	FGameplayEffectSpecHandle EffectSpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "DamageEffect")
	FDamageGameplayEffect DamageEffectOnOverlap;

	UFUNCTION()
	void OnImpactReceived(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:

	USG_AbilitySystemComponent* GetOwnerAbilitySystemComponent();

	void OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles);

	void OnHittableDestroyed();

};
