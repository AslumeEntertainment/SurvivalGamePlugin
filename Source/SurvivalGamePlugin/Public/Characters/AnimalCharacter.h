// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/InGameCharacter.h"
#include "AnimalCharacter.generated.h"

class UDamageDealingComponent;

UCLASS()
class SURVIVALGAMEPLUGIN_API AAnimalCharacter : public AInGameCharacter
{
	GENERATED_BODY()

public:

	AAnimalCharacter();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	UDamageDealingComponent* MouthDamageDealingComponent;


};
