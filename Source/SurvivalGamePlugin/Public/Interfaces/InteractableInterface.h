// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};


class SURVIVALGAMEPLUGIN_API IInteractableInterface
{
	GENERATED_BODY()

public:

	virtual bool InteractRequest(AController* InteractingCharacter) = 0;

	virtual bool bCanInteract(AController* InteractingCharacter) const = 0;

	virtual FText GetInteractableName() = 0;
};
