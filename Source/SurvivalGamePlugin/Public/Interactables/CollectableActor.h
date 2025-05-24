// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/SG_HittableActor.h"

#include "Interfaces/InteractableInterface.h"

#include "CollectableActor.generated.h"

class AItemBase;

UCLASS()
class SURVIVALGAMEPLUGIN_API ACollectableActor : public ASG_HittableActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:

	ACollectableActor();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	FText CollectableName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CustomValues")
	TSubclassOf<AItemBase> ItemToCollect = nullptr;

public:

	virtual bool InteractRequest(AController* InteractingCharacter) override;

	virtual bool bCanInteract(AController* InteractingCharacter) const override;

	virtual FText GetInteractableName() override;

};
