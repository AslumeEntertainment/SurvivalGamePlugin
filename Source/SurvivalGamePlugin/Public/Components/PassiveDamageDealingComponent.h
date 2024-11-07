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

protected:

	UPROPERTY()
	FGameplayEffectSpecHandle EffectSpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "DamageEffect")
	FDamageGameplayEffect DamageEffectOnOverlap;

	UFUNCTION()
	void OnImpactReceived(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
