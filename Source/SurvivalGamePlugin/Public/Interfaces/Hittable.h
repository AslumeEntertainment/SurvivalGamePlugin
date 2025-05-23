// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Hittable.generated.h"

class USG_AbilitySystemComponent;
class USceneComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UHittable : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALGAMEPLUGIN_API IHittable
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	virtual void OnComponentBeginTouch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) = 0;

	UFUNCTION()
	virtual void OnComponentHitReceived(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:

	UFUNCTION()
	virtual UShapeComponent* GetImpactChecker() = 0;

	UFUNCTION(BlueprintCallable)
	virtual USG_AbilitySystemComponent* GetSG_AbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsGuarding() const;

	UFUNCTION()
	virtual void DestroyHittable(bool bSafeDestruct = false) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool IsHitGuarded(FVector HitLocation) const;

	UFUNCTION(BlueprintCallable)
	virtual USceneComponent* GetBoundsComponent() = 0;

};
