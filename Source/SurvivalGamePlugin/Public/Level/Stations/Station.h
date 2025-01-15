// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interfaces/InteractableInterface.h"

#include "Station.generated.h"

class ASG_PlayerController;

UCLASS()
class SURVIVALGAMEPLUGIN_API AStation : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStation();

protected:

	UPROPERTY()
	ASG_PlayerController* InteractingPlayer = nullptr;

	UFUNCTION()
	virtual void StopInteracting();

	virtual bool InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;
	virtual FText GetInteractableName() override { return FText::FromString("Station"); }

public:
};
