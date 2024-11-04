// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Headers/DamageGameplayEffectStruct.h"
#include "PassiveDamageDealingComponent.generated.h"

class IHittableInterface;

UCLASS()
class SURVIVALGAMEPLUGIN_API UPassiveDamageDealingComponent : public UCapsuleComponent
{
	GENERATED_BODY()
	
public:

	UPassiveDamageDealingComponent();

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:

	UPROPERTY(EditDefaultsOnly, Category = "DamageEffect")
	FDamageGameplayEffect DefaultDamageEffect;

	TArray<IHittableInterface*> OverlapedActors;

	TArray<FGameplayEffectSpecHandle> InstantGameplayEffectSpecHandles;


public:

	TArray<IHittableInterface*> GetOverlappedActors() { return OverlapedActors; }

	void ApplyInstantGameplayEffectSpecHandle(FGameplayEffectSpecHandle SpecHandle, bool bShouldApplyOnOverlap = false);

};
