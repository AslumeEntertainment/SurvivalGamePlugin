// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/InGameCharacter.h"
#include "AnimalCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API AAnimalCharacter : public AInGameCharacter
{
	GENERATED_BODY()

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AnimalBiteAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AnimalScratchAction;

	void AttemptBite();
	void AttemptScratch();
};
