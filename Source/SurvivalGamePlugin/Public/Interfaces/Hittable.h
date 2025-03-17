// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Hittable.generated.h"

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
	virtual void OnComponentHitReceived(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


public:

	UFUNCTION()
	virtual void DestroyHittable(bool bSafeDestruct = false) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool IsGuarding() const;

};
